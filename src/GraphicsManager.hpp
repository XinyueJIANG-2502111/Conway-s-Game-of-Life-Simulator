#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.hpp"

// OpenGLを利用したセルの描画（インスタンス化描画）を管理するクラス
class GraphicsManager {
public:
    GraphicsManager() : VAO(0), VBO(0), EBO(0), instanceVBO(0), statusVBO(0) {}
    ~GraphicsManager() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &instanceVBO);
        glDeleteBuffers(1, &statusVBO);
    }

    // セルの配置位置の初期化
    void Init(const std::vector<glm::vec2>& positions);

    // 毎フレームの各セルの生存状態をGPUにアップロード
    void UpdateStatus(const std::vector<float>& states);

    // インスタンス化描画を実行
    void Draw(unsigned int instanceCount);

private:
    unsigned int VAO;
    unsigned int VBO, EBO;         // 基本図形（四角形）用のバッファ
    unsigned int instanceVBO;      // 各インスタンス（セル）の配置オフセット用のVBO
    unsigned int statusVBO;        // 各インスタンス（セル）の生死状態用のVBO
};


void GraphicsManager::Init(const std::vector<glm::vec2>& positions) {
    // 1. 基本となるセルの形状（1.0 x 1.0の四角形）を設定
    float vertices[] = {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);
    glGenBuffers(1, &statusVBO);

    glBindVertexArray(VAO);

    // --- 属性レイアウト 0: セルの基本頂点座標 (インスタンス間で共通) ---
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // --- 属性レイアウト 1: セルの配置オフセット (インスタンス間で固定の静的データ) ---
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glVertexAttribDivisor(1, 1); // 1インスタンス描画するごとにデータを1つ進める設定

    // --- 属性レイアウト 2: セルの生死状態 (フレームごとに変動する動的データ) ---
    glBindBuffer(GL_ARRAY_BUFFER, statusVBO);
    // 初期化時は領域確保のみ行い、データはNULL
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), nullptr, GL_STREAM_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1); // 1インスタンス描画するごとにデータを1つ進める設定

    glBindVertexArray(0);
}

void GraphicsManager::UpdateStatus(const std::vector<float>& states) {
    // 毎フレーム、セルの生死状態データをVBOへ高速転送
    glBindBuffer(GL_ARRAY_BUFFER, statusVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, states.size() * sizeof(float), states.data());
}

void GraphicsManager::Draw(unsigned int instanceCount) {
    // インスタンス化描画の実行
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instanceCount);
}