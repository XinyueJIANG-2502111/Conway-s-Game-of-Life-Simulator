#pragma once

#include "Shader.hpp"
#include "Camera.hpp"
#include "GraphicsManager.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

// ライフゲームのシミュレーションと状態管理を行うクラス
class Game {
public:
    // コンストラクタ（グリッドの幅と高さを指定）
    Game(int width, int height);

    // 入力処理（キーボード、マウス入力）
    void HandleInput(GLFWwindow* window);
    // ゲーム状態の更新（世代更新）
    void Update(float deltaTime);
    // ゲームのレンダリング
    void Render(int screenWidth, int screenHeight);
    // マウス左クリックによるセルの状態反転・配置
    void HandleMouseClick(GLFWwindow* window, int screenWidth, int screenHeight);

    // グリッドを全クリア
    void ClearGrid();

    // スクロール入力をカメラのズーム処理に伝達
    void ProcessScroll(float yoffset) {
        camera.ProcessMouseScroll(yoffset);
    }

private:
    // グリッドの解像度
    int gridWidth, gridHeight;
    // 一時停止フラグ
    bool isPaused = false;

    // ライフゲームの盤面データ（1次元配列で2次元盤面を表現）
    std::vector<float> currentGrid;
    std::vector<float> nextGrid;
    // セルの表示位置（ワールド座標系でのオフセット）
    std::vector<glm::vec2> cellPositions;

    // 描画関連のオブジェクト
    Shader shader;
    Camera camera;
    GraphicsManager graphics;

    // 世代更新の時間間隔（秒）とタイマー
    float updateInterval = 0.1f; // 0.1秒ごとに1世代更新
    float timer = 0.0f;

    // 特定のセル周囲の生存セル数をカウントするヘルパー関数
    int countNeighbors(int x, int y);
    // 2Dの(x, y)座標から1D配列のインデックスを取得するヘルパー関数
    int getIndex(int x, int y) { return y * gridWidth + x; }
};

Game::Game(int width, int height)
    : gridWidth(width), gridHeight(height),
    shader("Source/shaders/shader1.vs", "Source/shaders/shader1.fs"),
    camera(glm::vec2(0.0f, 0.0f), 0.05f) // 初期状態では全体が見えるようにカメラを引いておく
{
    currentGrid.resize(width * height, 0.0f);
    nextGrid.resize(width * height, 0.0f);

    // グリッド上のセルの位置（インスタンス用オフセット）を設定
    float spacing = 1.1f; // セル同士の間隔
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // 中心基準のローカル座標系にマッピング
            float posX = (x - width / 2.0f) * spacing;
            float posY = (y - height / 2.0f) * spacing;
            cellPositions.push_back(glm::vec2(posX, posY));

            // 初期配置：約20%の確率でセルを生存状態（1.0）にする
            currentGrid[getIndex(x, y)] = (rand() % 10 < 2) ? 1.0f : 0.0f;
        }
    }

    // グラフィックスマネージャーの初期化（セルの基本位置データをGPUへ転送）
    graphics.Init(cellPositions);
}

// 周囲8マスの生存セル数を数える（周期的境界条件ではなく、端はクリッピング処理）
int Game::countNeighbors(int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue; // 自分自身は除外

            int nx = x + i;
            int ny = y + j;

            // 境界チェック
            if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight) {
                if (currentGrid[getIndex(nx, ny)] > 0.5f) count++;
            }
        }
    }
    return count;
}

// 世代の更新ロジック
void Game::Update(float deltaTime) {
    if (isPaused) { return; } // 一時停止中は更新しない

    timer += deltaTime;
    if (timer < updateInterval) return; // 指定時間経過するまで待機
    timer = 0.0f;

    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            int neighbors = countNeighbors(x, y);
            int idx = getIndex(x, y);

            if (currentGrid[idx] > 0.5f) {
                // 生存ルール：周囲の生存セルが2個または3個の場合にのみ生存し続ける
                nextGrid[idx] = (neighbors == 2 || neighbors == 3) ? 1.0f : 0.0f;
            }
            else {
                // 誕生ルール：周囲の生存セルがちょうど3個の場合に新しく誕生する
                nextGrid[idx] = (neighbors == 3) ? 1.0f : 0.0f;
            }
        }
    }
    // 次世代を現在の盤面に反映
    currentGrid = nextGrid;
}

// 入力の処理
void Game::HandleInput(GLFWwindow* window) {
    // マウス右ボタン：ドラッグによるカメラ的平行移動
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (!camera.isPanning) camera.StartPanning(xpos, ypos);
        else camera.ProcessMouseMovement(xpos, ypos, 800.0f, 600.0f);
    }
    else {
        camera.StopPanning();
    }

    // スペースキー：一時停止／再開の切り替え
    static bool spaceWasPressed = false;
    bool spaceIsPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spaceIsPressed && !spaceWasPressed) {
        isPaused = !isPaused;
    }
    spaceWasPressed = spaceIsPressed;

    // マウス左クリック：セルの配置（ドロー操作）
    // 一時停止状態でも描画ができるように入力判定を行う
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        this->HandleMouseClick(window, width, height);
    }

    // Cキー：全セルのクリア
    static bool cWasPressed = false;
    bool cIsPressed = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;
    if (cIsPressed && !cWasPressed) {
        this->ClearGrid();
    }
    cWasPressed = cIsPressed;
}

// レンダリング処理
void Game::Render(int screenWidth, int screenHeight) {
    // 画面のクリア
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // シェーダーの使用とカメラ変換行列（MVP）の設定
    shader.use();
    shader.setMat4("transform", glm::value_ptr(camera.GetViewMatrix((float)screenWidth, (float)screenHeight)));

    // 最新のセル状態をGPUバッファに転送
    graphics.UpdateStatus(currentGrid);
    // セルをインスタンス描画
    graphics.Draw(currentGrid.size());
}

// マウス左クリック位置からグリッド座標への逆投影処理
void Game::HandleMouseClick(GLFWwindow* window, int screenWidth, int screenHeight) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // 1. マウスクリック位置を NDC (標準デバイス座標) に変換 (Y軸は上下反転)
        float x_ndc = (2.0f * (float)xpos) / screenWidth - 1.0f;
        float y_ndc = 1.0f - (2.0f * (float)ypos) / screenHeight;

        // 2. カメラのビュー行列の逆行列を使用して、ワールド座標系へ逆変換
        float aspectRatio = (float)screenWidth / screenHeight;
        glm::mat4 invTransform = glm::inverse(camera.GetViewMatrix(screenWidth, screenHeight));

        // 3. ワールド座標を算出
        glm::vec4 worldPos = invTransform * glm::vec4(x_ndc, y_ndc, 0.0f, 1.0f);

        // 4. セルのワールド配置間隔からグリッドの配列インデックスに逆マッピング
        float spacing = 1.1f;
        int gridX = std::round(worldPos.x / spacing + gridWidth / 2.0f);
        int gridY = std::round(worldPos.y / spacing + gridHeight / 2.0f);

        // 5. グリッドの有効範囲内であればセルを生存させる
        if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight) {
            currentGrid[getIndex(gridX, gridY)] = 1.0f;
        }
    }
}

void Game::ClearGrid() {
    // 使用 std::fill 将所有状态重置为死亡（0.0f）
    std::fill(currentGrid.begin(), currentGrid.end(), 0.0f);
    // 同时也清空下一帧缓存，防止交换后死而复生
    std::fill(nextGrid.begin(), nextGrid.end(), 0.0f);
}