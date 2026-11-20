#include "Game.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <exception>
#include <iostream>
#include <optional>

// 画面の初期解像度設定
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

namespace {
Game& gameForWindow(GLFWwindow* window) {
    return *static_cast<Game*>(glfwGetWindowUserPointer(window));
}

void registerInputCallbacks(GLFWwindow* window, Game& game) {
    glfwSetWindowUserPointer(window, &game);
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
    // --- 1. GLFW の初期化 ---
    if (!glfwInit()) {
        std::cerr << "GLFWの初期化に失敗しました。" << std::endl;
        return -1;
    }
    // OpenGLコンテキストのバージョンとプロファイルを設定（OpenGL 3.3 Core Profile）
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // --- 2. ウィンドウの作成 ---
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Conway's Life Game - OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cerr << "GLFWウィンドウの作成に失敗しました。" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // --- 3. GLAD の初期化（OpenGL関数ポインタのロード） ---
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLADの初期化に失敗しました。" << std::endl;
        glfwTerminate();
        return -1;
    }

    // --- 4. ゲームインスタンスの初期化 ---
    std::optional<Game> game;
    try {
        game.emplace(100, 100);
    } catch (const std::exception& error) {
        std::cerr << "Game initialization failed: " << error.what() << std::endl;
        glfwTerminate();
        return 1;
    }
    // --- 5. コールバック関数の登録 ---
    registerInputCallbacks(window, *game);

    // --- 6. メインレンダリングループ ---
    runGameLoop(window, *game);

    // --- 7. リソースの解放とクリーンアップ ---
    game.reset();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
