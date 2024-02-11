#ifndef Buffers_H
#define Buffers_H

#pragma once

#include "Base.h"
#include "Constants.h"
#include "Mesh.h"
#include "Misc.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"
#include "GameState.h"

#include <string>

class Mesh;
class Model;
class ModelInBuffer;
class MeshInBuffer;

class InstancedMeshBuffer {

    u32 VBO = 0;

    // Dynamic
    std::vector<m4> matrices;
    std::vector<v4> colors;

public:
    Mesh mesh;

    InstancedMeshBuffer(Mesh mesh);

    void Allocate();

    void UpdateMatrix(const u32 index, v3 size, v3 pos, f32 rotation = 0.0f)
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
        auto offset = matrices_size + (size * index);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            offset,
            size,
            &colors[index]);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Draw(Shader* shader, m4* projection, Texture* atlas);
    m4 GetMatrix(u32 index);

    MeshInBuffer AddMesh(v3 position, v3 size, v4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, f32 rotation = 0.0f, f32 scale = 1.0f);
    MeshInBuffer AddMesh(v3 position, v4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, f32 rotation = 0.0f, f32 scale = 1.0f);
};

// TODO: change to std::vector
static constexpr int MAX_CAPACITY = 2048;

class TexturedQuadBuffer {
    unsigned int VAO, VBO, EBO, instanced_VBO;

    m4 matrices[MAX_CAPACITY];
    v2 textures_coords[MAX_CAPACITY * 4];
    v4 colors[MAX_CAPACITY];

public:
    // TODO: make better.
    std::string texture_key;
	//TODO: make private
    u16 rolling_index = 0;

    void Allocate();

	void Reset();

    u16 GetCurrentIndex();

    u32 AddRect(const Rectangle rect);

    u32 AddTexturedRect(const AtlasTextureInfo* texture_info, const Texture* atlas, const v2 pos, const v2 size = { 0, 0 },
        const float rotation = 0, v4 color = { 1.0, 1.0, 1.0, 1.0 });

    void UpdateTexturedRect(u32 index, const AtlasTextureInfo* texture_info, const Texture* atlas, v2 pos, v2 size = { 0, 0 }, f32 rotation = 0.0f)
    {
        assert(index >= 0);
        assert(index <= MAX_CAPACITY);

        v2 size_to_use = size;

        if (size.x == 0 && size.y == 0)
            size_to_use = texture_info->size;

        this->matrices[index] = GetTransformMatrix(pos, size_to_use, rotation);

        float subtex_w = texture_info->size.x / atlas->Width;
        float subtex_h = texture_info->size.y / atlas->Height;
        float subtex_x = texture_info->position.x / atlas->Width;
        float subtex_y = texture_info->position.y / atlas->Height;

        int coords_index = index * 4;

        textures_coords[coords_index] = { subtex_x + subtex_w, subtex_y + subtex_h }; // TR
        textures_coords[coords_index + 1] = { subtex_x + subtex_w, subtex_y }; // BR
        textures_coords[coords_index + 2] = { subtex_x, subtex_y }; // BL
        textures_coords[coords_index + 3] = { subtex_x, subtex_y + subtex_h }; // TL

        UpdateBufferSection(index);
    }

    void UpdateColor(const int index, v4 color)
    {
        this->colors[index] = color;
        UpdateBufferSection(index);
    }

    void UpdateBufferSection(const int index)
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);

        assert(index >= 0);
        assert(index <= MAX_CAPACITY);

        auto size = sizeof(matrices[0]);
        auto offset = size * index;
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, &matrices[index]);

        size = 4 * sizeof(textures_coords[0]);
        offset = sizeof(matrices) + (size * index);
        auto coord_index = index * 4;
        glBufferSubData(
            GL_ARRAY_BUFFER,
            offset,
            size,
            &textures_coords[coord_index]);

        size = sizeof(colors[0]);
        offset = (sizeof(matrices) + sizeof(textures_coords)) + (size * index);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            offset,
            size,
            &colors[index]);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Draw(Shader* shader, m4* projection)
    {
        if (rolling_index == 0)
            return;

        shader->Use();
        //shader->setInt("imageSampler", 0);
        shader->setBool("hideAlpha", HIDE_ALPHA);
        shader->setMat4("projection", *projection);
		shader->setVec2("resolution", STATE.window.screen_size);
        auto atlas = RM.GetTexture(this->texture_key);

        if (atlas != nullptr) {
            glActiveTexture(GL_TEXTURE0);
            atlas->Bind();
        }

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, rolling_index);
        glBindVertexArray(0);
    }
};

#endif
