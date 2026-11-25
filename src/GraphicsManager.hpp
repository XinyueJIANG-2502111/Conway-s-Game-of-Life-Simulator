#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.hpp"

// Renders cells through OpenGL instancing.
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

    // Initialize buffers from the cell positions.
    void Init(const std::vector<glm::vec2>& positions);

    // Upload each cell's live state for the frame.
    void UpdateStatus(const std::vector<float>& states);

    // Draw the requested number of cell instances.
    void Draw(unsigned int instanceCount);

private:
    unsigned int VAO;
    unsigned int VBO, EBO;         // Shared quad vertices and indices.
    unsigned int instanceVBO;      // One position offset per cell instance.
    unsigned int statusVBO;        // One live state per cell instance.
};


void GraphicsManager::Init(const std::vector<glm::vec2>& positions) 
{
    // Center the unit quad at the origin for radial shading in cell space.
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

    // Attribute 0 supplies the quad vertices shared by all cells.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Attribute 1 holds fixed cell offsets so positions only need one upload.
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glVertexAttribDivisor(1, 1); // Advance the offset once per cell.

    // Attribute 2 streams cell states independently of the fixed geometry.
    glBindBuffer(GL_ARRAY_BUFFER, statusVBO);

    // Allocate space here and upload the first state before drawing.
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), nullptr, GL_STREAM_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1); // Advance the state once per cell.

    glBindVertexArray(0);
}

void GraphicsManager::UpdateStatus(const std::vector<float>& states) 
{
    // Refresh the state buffer in place for each frame.
    glBindBuffer(GL_ARRAY_BUFFER, statusVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, states.size() * sizeof(float), states.data());
}

void GraphicsManager::Draw(unsigned int instanceCount) 
{
    // Draw all cells with a single instanced draw call.
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instanceCount);
}
