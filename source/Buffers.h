#pragma once

//#include <glad/glad.h>
//#include <glm/gtx/transform.hpp>

#include "Base.h"
#include "Misc.h"
#include "Shader.h"
#include "Texture.h"

enum BufferType {
    None,
    Textured,
    Colored,
};

class Buffer {
protected:
    ID id;
    unsigned int VAO = 0, VBO = 0, EBO = 0, instanced_VBO = 0;
};

static constexpr int MAX_CAPACITY = 50;

class VertexBuffer : Buffer {
    m4 matrices[MAX_CAPACITY];
    v2 textures_coords[MAX_CAPACITY * 4];
    v4 colors[MAX_CAPACITY];

    u16 rolling_index = 0;

public:
    VertexBuffer()
    {
    }

    VertexBuffer(ID id)
    {
        this->id = id;

        constexpr float vertices[] = {
            0.5f, 0.5f, 0.0f, // top right
            0.5f, -0.5f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f, // bottom left
            -0.5f, 0.5f, 0.0f // top left
        };

        constexpr unsigned int indices[] = {
            // note that we start from 0!
            0, 1, 3, // first triangle
            1, 2, 3 // second triangle
        };

        // Gen
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &instanced_VBO);

        // Bind
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(matrices) + sizeof(textures_coords) + sizeof(colors), NULL, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(matrices), &matrices[0]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(matrices), sizeof(textures_coords), &textures_coords[0]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(matrices) + sizeof(textures_coords), sizeof(colors), &colors[0]);

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
        glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)sizeof(matrices));
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(matrices) + 2 * sizeof(float)));
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(matrices) + 4 * sizeof(float)));
        glEnableVertexAttribArray(10);
        glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(matrices) + 6 * sizeof(float)));
        glEnableVertexAttribArray(11);
        glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(matrices) + sizeof(textures_coords)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glVertexAttribDivisor(7, 1); // Tex coords
        glVertexAttribDivisor(8, 1); // Tex coords
        glVertexAttribDivisor(9, 1); // Tex coords
        glVertexAttribDivisor(10, 1); // Tex coords
        glVertexAttribDivisor(11, 1); // color
    }

    int AddTexturedRect(const AtlasTextureInfo* texture_info, const Texture* atlas, const v2 pos, const v2 size = { 0, 0 },
        const float rotation = 0)
    {
        assert(rolling_index >= 0);
        assert(rolling_index <= MAX_CAPACITY);

        v2 size_to_use = size;

        if (size.x == 0 && size.y == 0)
            size_to_use = texture_info->size;

        this->colors[rolling_index] = { 1.0, 1.0, 1.0, 1.0 }; // TODO: maybe extract to card
        this->matrices[rolling_index] = GetTransformMatrix(pos, size_to_use, rotation);

        float subtex_w = texture_info->size.x / atlas->Width;
        float subtex_h = texture_info->size.y / atlas->Height;
        float subtex_x = texture_info->position.x / atlas->Width;
        float subtex_y = texture_info->position.y / atlas->Height;

        int coords_index = rolling_index * 4;

        textures_coords[coords_index] = { subtex_x + subtex_w, subtex_y + subtex_h }; // TR
        textures_coords[coords_index + 1] = { subtex_x + subtex_w, subtex_y }; // BR
        textures_coords[coords_index + 2] = { subtex_x, subtex_y }; // BL
        textures_coords[coords_index + 3] = { subtex_x, subtex_y + subtex_h }; // TL

        UpdateBufferSection(rolling_index);

        const int tmp = rolling_index;
        rolling_index += 1;
        return tmp;
    }

    int AddRect(const Rectangle* rect)
    {
        assert(rolling_index >= 0);
        assert(rolling_index <= MAX_CAPACITY);

        this->colors[rolling_index] = rect->color;
        this->matrices[rolling_index] = GetTransformMatrix(rect->transform.position, rect->transform.size, rect->transform.rotation);

        UpdateBufferSection(rolling_index);

        const int tmp = rolling_index;
        rolling_index += 1;
        return tmp;
    }

    void UpdateMatrix(const int index, v2 size, v3 pos, float rotation = 0.0f)
    {
        this->matrices[index] = GetTransformMatrix(pos, size, rotation);
    }

    void UpdateColor(const int index, v4 color)
    {
        this->colors[index] = color;
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
};
