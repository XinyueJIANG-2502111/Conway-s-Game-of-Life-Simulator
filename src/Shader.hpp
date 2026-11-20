#pragma once

#include <glad/glad.h>
#include <stdexcept>
#include <string>

class Shader {
public:
    Shader(const char* vertexSource, const char* fragmentSource) 
    {
        GLuint vertex = Compile(GL_VERTEX_SHADER, vertexSource, "shader1.vs");
        GLuint fragment;

        try {
            fragment = Compile(GL_FRAGMENT_SHADER, fragmentSource, "shader1.fs");       
        } catch (...) {
            glDeleteShader(vertex);
            throw;
        }

        program = glCreateProgram();
        
        if (program == 0) {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            throw std::runtime_error("Could not create the shader program");
        }

        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        GLint linked = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (linked != GL_TRUE) {
            char log[1024]{};
            glGetProgramInfoLog(program, sizeof(log), nullptr, log);
            glDeleteProgram(program);
            throw std::runtime_error(std::string("Could not link the shader program: ") + log);
        }
    }

    ~Shader() {
        glDeleteProgram(program);
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() {
        glUseProgram(program);
    }

    void setMat4(const std::string& name, const float* mat) const {
        glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, mat);
    }

private:
    GLuint program;

    static GLuint Compile(GLenum type, const char* source, const char* name) {
        GLuint shader = glCreateShader(type);
        if (shader == 0) {
            throw std::runtime_error(std::string("Could not create ") + name);
        }

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint compiled = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled != GL_TRUE) {
            char log[1024]{};
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            glDeleteShader(shader);
            throw std::runtime_error(std::string("Could not compile ") + name + ": " + log);
        }
        return shader;
    }
};
