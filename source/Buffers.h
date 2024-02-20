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
class MeshInBuffer;

static constexpr int MAX_CAPACITY = 2048;

enum class BufferElementType {
	Bool,
	VBool2,
	VBool3,
	VBool4,

	Int,
	VInt2,
	VInt3,
	VInt4,

	Float,
	VFloat2,
	VFloat3,
	VFloat4,
};

GLenum BufferElementTypeToOpenGLType(BufferElementType type);
u8 GetBufferElementSize(BufferElementType type);
u8 GetBufferElementTypeLength(BufferElementType type);

struct BufferElement {
	BufferElementType type;
	std::string name;
	u64         size;
	u8          length;
	u64         offset;

	BufferElement(BufferElementType type, std::string name) {
		this->type = type;
		this->name = name;
		size   = GetBufferElementSize(type);
		length = GetBufferElementTypeLength(type);
	}
};

struct BufferLayout {
	std::vector<BufferElement> elements;
	u64 size = 0;

    BufferLayout() { }
	BufferLayout(std::initializer_list<BufferElement> items) : elements(items){
		CalculateStrideAndTotalSize();
	}

	void CalculateStrideAndTotalSize() {
		u64 offset = 0;
		for (BufferElement& el : elements) {
			el.offset  = offset;
			this->size += el.size;
			offset     += el.size;
		}
	}
};


class UniversalBuffer {
    public:
	std::string  buffer_name, shader_name, texture_name;
        BufferLayout layout;

        UniversalBuffer() {}
        UniversalBuffer(std::string buffer_name, std::string shader_name, std::string texture_name, BufferLayout layout)
        {
			this->buffer_name  = buffer_name;
			this->shader_name  = shader_name;
			this->texture_name = texture_name;
			this->layout = layout;
        }

        virtual void SetData(void* data, u32 size) {};
        virtual void Bind() {};
        virtual void Unbind() {};
        virtual BufferLayout& GetLayout() = 0;
};

class OpenGlBuffer : public UniversalBuffer {
    unsigned int VAO, VBO, EBO, instanced_VBO;

public:
	OpenGlBuffer(u32 buffer_size, std::string buffer_name, std::string shader_name, std::string texture_name, BufferLayout layout) : UniversalBuffer(buffer_name, shader_name, texture_name, layout) { 

		constexpr f32 vertices[] = {
			0.5f, 0.5f, 0.0f, // top right
			0.5f, -0.5f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f, // bottom left
			-0.5f, 0.5f, 0.0f // top left
		};

		constexpr u32 indices[] = {
			// note that we start from 0!
			0, 1, 3, // first triangle
			1, 2, 3 // second triangle
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenBuffers(1, &instanced_VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		
		glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);
		glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_DYNAMIC_DRAW);

		u8 position = 3;
		for (BufferElement& el : layout.elements) {
			GLenum gl_type = BufferElementTypeToOpenGLType(el.type);
			printf("glEnableVertexAttribArray(%i)\n", position);
			printf("glVertexAttribPointer(%i, %i, %i, GL_FALSE, %u, (void*)%u);\n", position, el.length, gl_type, layout.size, el.offset);
			printf("glVertexAttribDivisor(%i, 1)\n", position);
			printf("\n");
			
			glEnableVertexAttribArray(position);
			glVertexAttribPointer(position, el.length, gl_type, GL_FALSE, layout.size, (void*)el.offset);
			glVertexAttribDivisor(position, 1);
			position++;
		}

		Unbind();
	}

	virtual BufferLayout& GetLayout() {
		return layout;
	}

	virtual void SetData(void* data, u32 size) {
		Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		Unbind();
	};

	virtual void Bind() {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);
	};

	virtual void Unbind() {
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	};

};

//------------------------------------------------------------------------

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

	u32 AddQuad(const v2 position, const v2 size, const v4 color, float rotation = 0);

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
        shader->setInt("imageSampler", 0);
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
