#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// カメラ操作を管理するクラス
class Camera {
public:
    // カメラのプロパティ（位置とズーム倍率）
    glm::vec2 Position;
    float Zoom;

    // ドラッグ操作の状態と直前のマウス位置
    bool isPanning;
    glm::vec2 LastMousePos;

    Camera(glm::vec2 position = glm::vec2(0.0f, 0.0f), float zoom = 1.0f)
        : Position(position), Zoom(zoom), isPanning(false), LastMousePos(0.0f) {
    }

    // ビュー・射影行列（MVP）を取得
    glm::mat4 GetViewMatrix(float screenWidth, float screenHeight) {
        float aspectRatio = screenWidth / screenHeight;
        // 2Dシーンに適した正投影（オルト）マトリクスを生成
        glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        // カメラのズーム（スケーリング）と位置（平行移動）の適用
        view = glm::scale(view, glm::vec3(Zoom, Zoom, 1.0f));
        view = glm::translate(view, glm::vec3(Position.x, Position.y, 0.0f));
        return projection * view;
    }

    // マウススクロールによるズーム処理
    void ProcessMouseScroll(float yoffset) {
        if (yoffset > 0)
            Zoom *= 1.1f;
        else
            Zoom /= 1.1f;

        // ズーム値が小さくなりすぎないように制御
        if (Zoom < 0.01f) Zoom = 0.01f;
    }

    // ドラッグ（平行移動）の開始（マウス右ボタン押下時）
    void StartPanning(double xpos, double ypos) {
        isPanning = true;
        LastMousePos = glm::vec2(xpos, ypos);
    }

    // ドラッグ（平行移動）の終了
    void StopPanning() {
        isPanning = false;
    }

    // マウスの移動量に応じてカメラ位置を更新する
    void ProcessMouseMovement(double xpos, double ypos, float screenWidth, float screenHeight) {
        float aspectRatio = screenWidth / screenHeight;
        if (isPanning) {
            glm::vec2 currentPos = glm::vec2(xpos, ypos);
            glm::vec2 delta = currentPos - LastMousePos;

            // マウスのスクリーン移動量を OpenGL の NDC（-1〜1）および現在のカメラズームに変換
            float offsetX = (delta.x / screenWidth) * 2.0f * aspectRatio / Zoom;
            float offsetY = -(delta.y / screenHeight) * 2.0f / Zoom;

            Position += glm::vec2(offsetX, offsetY);
            LastMousePos = currentPos;
        }
    }
};