#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// シェーダーの読み込み、コンパイル、リンクを管理するクラス
class Shader {
public:
    unsigned int ID; // シェーダープログラムのID

    // コンストラクタ（頂点シェーダーとフラグメントシェーダーのパスを指定）
    Shader(const char* vertexPath, const char* fragmentPath) {
        std::cout << "シェーダーを読み込んでいます..." << std::endl;
        
        // 1. ファイルシステムからシェーダーコードを読み込む
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        // 読み込み失敗時に例外を投げるように設定
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // ファイルのバッファ内容をストリームへ読み込む
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            // ストリームから文字列に変換
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cout << "エラー::シェーダー::ファイルの読み込みに失敗しました: " << e.what() << std::endl;
        }

        const char* vShaderSource = vertexCode.c_str();
        const char* fShaderSource = fragmentCode.c_str();

        // 2. シェーダーのコンパイルとプログラムのリンク
        unsigned int vertex, fragment;

        // 頂点シェーダーのコンパイル
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderSource, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        // フラグメントシェーダーのコンパイル
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderSource, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // シェーダープログラムのリンク
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        // リンク後は不要になった個別シェーダーオブジェクトを削除
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        std::cout << "シェーダーが正常にロードされました！" << std::endl;
    }

    // シェーダープログラムのアクティブ化
    void use() {
        glUseProgram(ID);
    }

    // Uniformマトリクス(mat4)変数の設定用ヘルパー関数
    void setMat4(const std::string& name, const float* mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat);
    }

private:
    // コンパイルおよびリンクエラーのチェック用関数
    void checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "エラー::シェーダー::コンパイルエラー (タイプ: " << type << ")\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "エラー::プログラム::リンクエラー (タイプ: " << type << ")\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};