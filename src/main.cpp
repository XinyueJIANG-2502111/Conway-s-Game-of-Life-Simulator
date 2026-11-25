#include "Game.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <exception>
#include <iostream>
#include <optional>

// Initial window size in screen coordinates.
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

namespace {
Game& gameForWindow(GLFWwindow* window) {
    return *static_cast<Game*>(glfwGetWindowUserPointer(window));
}

void registerInputCallbacks(GLFWwindow* window, Game& game) {
    glfwSetWindowUserPointer(window, &game);
    // Press callbacks catch inputs that begin and end during a single event poll.
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int, int action, int) {
        if (action != GLFW_PRESS) return;
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } else {
            gameForWindow(window).HandleKeyPress(key);
        }
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double, double yoffset) {
        gameForWindow(window).ProcessScroll(yoffset);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int) {
        gameForWindow(window).HandleMouseButton(window, button, action);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
        gameForWindow(window).HandleCursorPosition(window, x, y);
    });
    glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int entered) {
        if (!entered) gameForWindow(window).HandleCursorLeave();
    });
    glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused) {
        if (!focused) gameForWindow(window).ResetPointerPath();
    });

    // Either size change can invalidate the cursor-to-grid mapping.
    const auto resetAfterResize = [](GLFWwindow* window, int, int) {
        gameForWindow(window).ResetPointerPath();
    };
    glfwSetWindowSizeCallback(window, resetAfterResize);
    glfwSetFramebufferSizeCallback(window, resetAfterResize);
}

void runGameLoop(GLFWwindow* window, Game& game) {
    while (!glfwWindowShouldClose(window)) {
        game.Tick();
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) break;

        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        const bool canRender = framebufferWidth > 0 && framebufferHeight > 0 &&
            !glfwGetWindowAttrib(window, GLFW_ICONIFIED);
        if (canRender) {
            game.PaintHeldCell();
            game.Render(framebufferWidth, framebufferHeight);
            glfwSwapBuffers(window);
        }

        if (glfwWindowShouldClose(window)) break;
        if (game.IsPaused()) {
            glfwWaitEvents();
        } else if (!canRender) {
            // A running simulation still needs its next generation while hidden.
            const double timeout = game.SecondsUntilNextGeneration();
            // GLFW's Win32 wait rounds down to milliseconds.
            if (timeout > 0.0) glfwWaitEventsTimeout(std::max(timeout, 0.001));
        }
    }
}
}

int main() {
    // Initialize GLFW before creating the window.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }
    // Request the OpenGL 3.3 core context expected by the shaders.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window and make its context current.
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Conway's Life Game - OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create a GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load OpenGL entry points from the current context.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD." << std::endl;
        glfwTerminate();
        return -1;
    }

    // Initialize the simulation and its renderer.
    std::optional<Game> game;
    try {
        game.emplace(100, 100);
    } catch (const std::exception& error) {
        std::cerr << "Game initialization failed: " << error.what() << std::endl;
        glfwTerminate();
        return 1;
    }
    // Register callbacks once the game instance is ready.
    registerInputCallbacks(window, *game);

    // Run the simulation and render loop.
    runGameLoop(window, *game);

    // Release GPU resources before destroying the current context.
    game.reset();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
