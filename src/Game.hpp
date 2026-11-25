#pragma once

#include "Shader.hpp"
#include "Camera.hpp"
#include "GraphicsManager.hpp"
#include "GameShaders.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <optional>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

// Owns the Life simulation and its state.
class Game {
public:
    // Create a board with the given width and height.
    Game(int width, int height);

    // Keyboard and mouse input handlers.
    void HandleKeyPress(int key);

    void PaintHeldCell();

    void HandleMouseButton(GLFWwindow* window, int button, int action);

    void HandleCursorPosition(GLFWwindow* window, double x, double y);

    void HandleCursorLeave();

    void ResetPointerPath();

    // Schedule and advance simulation generations.
    void Tick();

    void Update(std::chrono::nanoseconds elapsed);

    bool IsPaused() const { return isPaused; }

    double SecondsUntilNextGeneration() const;

    // Render into the active framebuffer.
    void Render(int framebufferWidth, int framebufferHeight);

    // Clear the entire board.
    void ClearGrid();

    // Forward scroll input to the camera and break the stroke before zooming.
    void ProcessScroll(double yoffset) {
        if (std::isfinite(yoffset) && yoffset != 0.0) 
            strokePoint.reset();
        
        camera.ProcessMouseScroll(yoffset);
    }

private:
    using Clock = std::chrono::steady_clock;
    static constexpr float cellSpacing = 1.1f;

    void PaintStroke(glm::dvec2 from, glm::dvec2 to);
    void AdvanceGeneration();

    // Board dimensions in cells.
    int gridWidth, gridHeight;

    // Pause generation updates while allowing edits.
    bool isPaused = false;
    bool isPainting = false;
    std::optional<glm::dvec2> cursorPosition;
    std::optional<glm::dvec2> strokePoint;

    // Flat row-major buffers for the current and next generations.
    std::vector<float> currentGrid;
    std::vector<float> nextGrid;
    // Per-cell positions in world space.
    std::vector<glm::vec2> cellPositions;

    // Resources used to render the board.
    Shader shader;
    Camera camera;
    GraphicsManager graphics;

    static constexpr auto generationInterval = std::chrono::milliseconds(100);
    static constexpr std::chrono::nanoseconds::rep maxGenerationsPerFrame = 5;
    std::chrono::nanoseconds generationRemainder{};
    Clock::time_point lastTick;

    // Count live neighbors around a cell.
    int countNeighbors(int x, int y);
    // Map board coordinates to a row-major buffer index.
    int getIndex(int x, int y) { return y * gridWidth + x; }
};

Game::Game(int width, int height)
    : gridWidth(width), gridHeight(height),
    shader(GameShaders::vertex, GameShaders::fragment),
    camera(glm::vec2(0.0f, 0.0f), 0.05f) // Start zoomed out for a wider view.
{
    currentGrid.resize(width * height, 0.0f);
    nextGrid.resize(width * height, 0.0f);

    // Precompute the instance offsets because the board layout is fixed.
    for (int y = 0; y < height; ++y) 
    {
        for (int x = 0; x < width; ++x) 
        {
            // Use the grid midpoint as the world-space origin.
            float posX = (x - width / 2.0f) * cellSpacing;
            float posY = (y - height / 2.0f) * cellSpacing;

            cellPositions.push_back(glm::vec2(posX, posY));

            // Seed roughly one in five cells as alive.
            currentGrid[getIndex(x, y)] = (rand() % 10 < 2) ? 1.0f : 0.0f;
        }
    }

    // Upload the fixed cell positions to the GPU.
    graphics.Init(cellPositions);

    // Blend the cells' soft edges using premultiplied alpha.
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // Start timing after GPU setup so loading time does not advance the board.
    lastTick = Clock::now();
}

// Count live cells in the eight surrounding positions without wrapping at edges.
int Game::countNeighbors(int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; ++i) 
    {
        for (int j = -1; j <= 1; ++j) 
        {
            if (i == 0 && j == 0) continue; // Exclude the center cell.

            int nx = x + i;
            int ny = y + j;

            // Ignore neighbors outside the board.
            if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight) {
                if (currentGrid[getIndex(nx, ny)] > 0.5f) count++;
            }
        }
    }
    return count;
}

// Advance the simulation using elapsed time from the steady clock.
void Game::Tick() {
    const auto now = Clock::now();

    const auto elapsed = 
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastTick);

    lastTick = now;

    Update(elapsed);
}

double Game::SecondsUntilNextGeneration() const {
    const auto untilNextGeneration = 
        generationInterval - generationRemainder - (Clock::now() - lastTick);

    return std::chrono::duration<double>(untilNextGeneration).count();
}

void Game::Update(std::chrono::nanoseconds elapsed) {
    if (isPaused) return;

    // Drop excess full generations after a long frame, but keep the fractional time.
    generationRemainder += elapsed % generationInterval;

    const auto due = 
        elapsed / generationInterval + 
        generationRemainder / generationInterval;

    generationRemainder %= generationInterval;

    const auto generationsToRun = std::min(due, maxGenerationsPerFrame);

    for (int step = 0; step < generationsToRun; ++step) 
    {
        AdvanceGeneration();
    }
}

void Game::AdvanceGeneration() {
    // Write to a separate buffer so every cell reads the same generation.
    for (int y = 0; y < gridHeight; ++y) 
    {
        for (int x = 0; x < gridWidth; ++x) 
        {
            int neighbors = countNeighbors(x, y);
            int idx = getIndex(x, y);

            if (currentGrid[idx] > 0.5f) {
                // A live cell survives with two or three live neighbors.
                nextGrid[idx] = (neighbors == 2 || neighbors == 3) ? 1.0f : 0.0f;
            }
            else {
                // A dead cell comes to life with exactly three live neighbors.
                nextGrid[idx] = (neighbors == 3) ? 1.0f : 0.0f;
            }
        }
    }
    // Publish the completed generation by swapping buffers.
    currentGrid.swap(nextGrid);
}

void Game::HandleKeyPress(int key) {
    if (key == GLFW_KEY_SPACE) {
        // Account for elapsed time under the old state before changing it.
        Tick();
        isPaused = !isPaused;
    } else if (key == GLFW_KEY_C) {
        ClearGrid();
    }
}

void Game::PaintHeldCell() {
    // Keep painting the endpoint while the left button is held and the cursor is still.
    if (strokePoint) PaintStroke(*strokePoint, *strokePoint);
}

void Game::HandleMouseButton(GLFWwindow* window, int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) 
    {
        isPainting = action == GLFW_PRESS;
        strokePoint.reset();
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) 
    {
        camera.StopPanning();
    } else 
    {
        return;
    }

    if (action != GLFW_PRESS) return;

    // Prefer the latest cursor event so a queued click uses that event's position.
    glm::dvec2 cursor = cursorPosition.value_or(glm::dvec2(0.0));

    if (!cursorPosition) glfwGetCursorPos(window, &cursor.x, &cursor.y);

    HandleCursorPosition(window, cursor.x, cursor.y);
}

void Game::HandleCursorLeave() {
    cursorPosition.reset();
    strokePoint.reset();
}

void Game::ResetPointerPath() {
    HandleCursorLeave();
    camera.StopPanning();
}

void Game::HandleCursorPosition(GLFWwindow* window, double x, double y) {
    if (!std::isfinite(x) || !std::isfinite(y)) {
        ResetPointerPath();
        return;
    }
    cursorPosition = glm::dvec2(x, y);

    const bool isPanning = 
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (!isPainting && !isPanning) return;

    int windowWidth, windowHeight, framebufferWidth, framebufferHeight;

    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    if (windowWidth <= 0 || windowHeight <= 0 || 
        framebufferWidth <= 0 || framebufferHeight <= 0) 
    {
        ResetPointerPath();
        return;
    }

    // GLFW cursor coordinates use window units. Projection uses framebuffer pixels.
    const double aspectRatio = static_cast<double>(framebufferWidth) / framebufferHeight;
    if (isPanning) 
    {
        camera.ProcessMouseMovement(*cursorPosition, glm::dvec2(windowWidth, windowHeight), aspectRatio);
    }
    if (!isPainting) return;

    if (x < 0.0 || x >= windowWidth || y < 0.0 || y >= windowHeight) 
    {
        strokePoint.reset();
        return;
    }

    const glm::dvec2 ndc(2.0 * x / windowWidth - 1.0, 1.0 - 2.0 * y / windowHeight);
    const glm::dvec2 gridCenter(gridWidth / 2.0, gridHeight / 2.0);

    // Offset by half a cell so the cell's integer index starts at its lower edge.
    const glm::dvec2 point = 
        camera.NdcToWorld(ndc, aspectRatio) / static_cast<double>(cellSpacing) 
        + gridCenter + glm::dvec2(0.5);

    if (!std::isfinite(point.x) || 
        !std::isfinite(point.y)) 
    {
        strokePoint.reset();
        return;
    }

    PaintStroke(strokePoint.value_or(point), point);
    strokePoint = point;
}

// Render one frame with the current camera and board state.
void Game::Render(int framebufferWidth, int framebufferHeight) {
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    // Clear the framebuffer to the color visible between cells.
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Bind the shader and supply the camera transform.
    shader.use();
    const float aspectRatio = static_cast<float>(framebufferWidth) / framebufferHeight;
    shader.setMat4("transform", glm::value_ptr(camera.GetViewMatrix(aspectRatio)));

    // Upload the latest board state before drawing.
    graphics.UpdateStatus(currentGrid);
    // Render the board as instances of the shared quad.
    graphics.Draw(currentGrid.size());
}

void Game::PaintStroke(glm::dvec2 from, glm::dvec2 to) {
    if (gridWidth <= 0 || gridHeight <= 0) return;

    // Clip the segment to the board before converting its endpoints to cell indices.
    const glm::dvec2 delta = to - from;

    double entry = 0.0, exit = 1.0;

    auto clipAxis = [&](double origin, double direction, double limit) 
    {
        if (direction == 0.0) 
            return (origin >= 0.0 && origin < limit);

        const double a = -origin / direction;
        const double b = (limit - origin) / direction;

        entry = std::max(entry, std::min(a, b));
        exit = std::min(exit, std::max(a, b));

        return entry <= exit;
    };

    if (!clipAxis(from.x, delta.x, gridWidth) || 
        !clipAxis(from.y, delta.y, gridHeight)) return;

    auto toIndex = [](double position, int size) 
    {
        return static_cast<int>(
            std::clamp(
                position, 0.0, static_cast<double>(size - 1)));
    };

    const glm::dvec2 start = from + delta * entry;
    const glm::dvec2 end = from + delta * exit;
    
    int x = toIndex(start.x, gridWidth);
    int y = toIndex(start.y, gridHeight);

    const int endX = toIndex(end.x, gridWidth);
    const int endY = toIndex(end.y, gridHeight);

    // Bresenham fills the cells between consecutive cursor samples in any direction.
    const int dx = std::abs(endX - x);
    const int dy = -std::abs(endY - y);

    const int stepX = x < endX ? 1 : -1;
    const int stepY = y < endY ? 1 : -1;

    int error = dx + dy;

    while (true) 
    {
        currentGrid[getIndex(x, y)] = 1.0f;

        if (x == endX && y == endY) break;

        const int twiceError = 2 * error;

        if (twiceError >= dy) {
            error += dy;
            x += stepX;
        }

        if (twiceError <= dx) {
            error += dx;
            y += stepY;
        }
    }
}

void Game::ClearGrid() 
{
    // Use std::fill to reset all cell states to dead (0.0f).
    std::fill(currentGrid.begin(), currentGrid.end(), 0.0f);

    // Also clear the next-frame buffer to prevent dead cells from coming back to life after buffer swapping.
    std::fill(nextGrid.begin(), nextGrid.end(), 0.0f);
}
