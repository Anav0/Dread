#ifndef Buffers_H
#define Buffers_H

#pragma once

#include "Base.h"
#include "Constants.h"
#include "Misc.h"
#include "Shader.h"
#include "Texture.h"

#include <string>

class Buffer {
protected:
    ID id;
    unsigned int VAO = 0, VBO = 0, EBO = 0, instanced_VBO = 0;
};

static constexpr int MAX_CAPACITY = 50;

class Mesh;
class Model;
class ModelInBuffer;

class MeshBuffer : public Buffer {
    // Static
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    // Dynamic
    std::vector<m4> matrices;
    std::vector<v4> colors;

    u16 rolling_index;

    std::map<std::string, u32> mesh_id_by_vertices_index;
    std::map<std::string, u32> mesh_id_by_indices_index;

public:
    MeshBuffer() { }

    MeshBuffer(std::vector<std::string>&, ID id);

    void AllocateBufferData() {
        //TODO: Nie wiadomo o ile trzeba "skoczyc"
        //
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(matrices) + sizeof(colors), NULL, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(matrices), &matrices[0]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(matrices), sizeof(colors), &colors[0]);
    }

    void Initialize() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &instanced_VBO);

        // Bind
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        // texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        // tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

        // bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

        //----------------------
        // Instanced buffer data
        //----------------------

        glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);

        // vec3 position
        std::size_t vec4Size = sizeof(v4);

        auto stride = 4 * sizeof(v4);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void*)(1 * vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * vec4Size));
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(v4), (void*)sizeof(matrices));

        glVertexAttribDivisor(3, 1); // Matrix transform
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1); // Color
    }

    void UpdateMatrix(const u32 index, v3 size, v3 pos, float rotation = 0.0f) 
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);

        assert(index >= 0);
        this->matrices[index] = GetTransformMatrix(pos, size, rotation);

        auto offset = sizeof(m4) * index;
        glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(m4), &matrices[index]);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void UpdateMatrix(const u32 index, m4 matrix) 
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);

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
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);

        assert(index >= 0);
        this->colors[index] = color;

        auto size = sizeof(v4);
        auto offset = sizeof(matrices) + (size * index);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            offset,
            size,
            &colors[index]);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Draw(Shader* shader, m4* projection, Texture* atlas)
    {
        if (rolling_index == 0)
            return;

        if (atlas != nullptr) {
            glActiveTexture(GL_TEXTURE0);
            atlas->Bind();
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);

        shader->Use();
        shader->setInt("imageSampler", 0);
        shader->setMat4("projection", *projection);

        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, rolling_index);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ModelInBuffer RenderModel(std::string model_index, v3 position, v3 size, f32 rotation = 0.0f);
};

#endif
