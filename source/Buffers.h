#ifndef Buffers_H
#define Buffers_H

#pragma once

#include "Base.h"
#include "Constants.h"
#include "Misc.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

#include <string>

static constexpr int MAX_CAPACITY = 50;

class Mesh;
class Model;
class ModelInBuffer;
class MeshInBuffer;

class InstancedBuffer {

    unsigned int VBO = 0;

    // Static
    Mesh mesh;

    // Dynamic
    std::vector<m4> matrices;
    std::vector<v4> colors;

public:
    InstancedBuffer(Mesh mesh);

    void Allocate();

    void UpdateMatrix(const u32 index, v3 size, v3 pos, float rotation = 0.0f) 
    {
        glBindVertexArray(mesh.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        assert(index >= 0);
        this->matrices[index] = GetTransformMatrix(pos, size, rotation);

        auto offset = sizeof(m4) * index;
        glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(m4), &matrices[index]);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void UpdateMatrix(const u32 index, m4 matrix) 
    {
        glBindVertexArray(mesh.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        assert(index >= 0);
        matrices[index] = matrix;

        auto size = sizeof(m4);
        auto offset = size * index;
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, &matrices[index]);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void UpdateColor(const int index, v4 color)
    {
        glBindVertexArray(mesh.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        assert(index >= 0);
        this->colors[index] = color;

        auto matrices_size = sizeof(m4) * matrices.size();

        auto size = sizeof(v4);
        auto offset = matrices_size  + (size * index);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            offset,
            size,
            &colors[index]);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Draw(Shader* shader, m4* projection, Texture* atlas);

    MeshInBuffer AddMesh(v3 position, v3 size, v4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, f32 rotation = 0.0f, f32 scale = 1.0f);
};

#endif
