#include "Game.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// コールバック関数の宣言
// ウィンドウサイズが変更されたときに呼び出される関数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// マウスホイールがスクロールされたときに呼び出される関数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// 画面の初期解像度設定
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// グローバルなライフゲームインスタンスへのポインタ（コールバック関数用）
Game* lifeGame = nullptr;

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

    // --- 3. コールバック関数の登録 ---
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // --- 4. GLAD の初期化（OpenGL関数ポインタのロード） ---
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLADの初期化に失敗しました。" << std::endl;
        return -1;
    }

    // --- 5. ゲームインスタンスの初期化 ---
    // 100x100 のグリッドサイズでライフゲームを作成
    lifeGame = new Game(100, 100);

    // フレーム時間の計測用変数
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // --- 6. メインレンダリングループ ---
    while (!glfwWindowShouldClose(window)) {
        // 現在のフレームバッファサイズを取得
        int scr_width, scr_height;
        glfwGetFramebufferSize(window, &scr_width, &scr_height);

        // デルタタイム（フレームの経過時間）の計算
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // A. 入力処理
        // ESCキーが押されたらウィンドウを閉じる
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        lifeGame->HandleInput(window);

        // B. ゲームロジック（世代更新）
        lifeGame->Update(deltaTime);

        // C. レンダリングの実行
        lifeGame->Render(scr_width, scr_height);

        // カラーバッファの入れ替えとイベントのポーリング
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- 7. リソースの解放とクリーンアップ ---
    delete lifeGame;
    glfwTerminate();

    return 0;
}

// ---------------------------------------------------------
// コールバック関数の実装
// ---------------------------------------------------------

// ビューポートサイズをウィンドウサイズに追従させる
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// マウススクロールイベントをGameクラスのカメラに伝える
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (lifeGame) {
        lifeGame->ProcessScroll(yoffset);
    }
}