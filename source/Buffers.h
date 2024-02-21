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
#include <array>

class Mesh;
class Model;
class MeshInBuffer;

static constexpr int MAX_CAPACITY = 2048;

enum class BufferElementType {
	Bool,
	VBool2,
	VBool3,
	VBool4,

	UInt,
	VUInt2,
	VUInt3,
	VUInt4,

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

	bool IsInt() {
		return type == BufferElementType::Int ||
			   type == BufferElementType::UInt ||
			   type == BufferElementType::VInt2 ||
			   type == BufferElementType::VInt3 ||
			   type == BufferElementType::VInt4 ||
			   type == BufferElementType::VUInt2 ||
			   type == BufferElementType::VUInt3 ||
			   type == BufferElementType::VUInt4;
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

enum class GradientType : i32 {
	Radial = 0,
	ThreeColor = 1,
};

constexpr u8 GRADIENT_MAX_COLORS = 4;
struct GradientBufferElement {
	m4 matrices;
	u32 gradient_type;
	v2 middle;
    std::array<v4, GRADIENT_MAX_COLORS> colors;
};

struct Gradient {
	GradientType gradient_type;
	v2 middle;
    std::array<v4, GRADIENT_MAX_COLORS> colors;
};

class GradientBuffer {
    unsigned int VAO, VBO, EBO, instanced_VBO;

	GradientBufferElement elements[MAX_CAPACITY];

public:
    u16 rolling_index = 0;

    void Allocate(u32 buffer_size, BufferLayout);
	void Flush();
	void Draw(Shader* shader, m4* projection);
	void Reset();
	
	u32 AddGradient(const v2 pos, const v2 size, const Gradient gradient);
};

struct TexturedQuadBufferElement {
	m4 matrices;
	v2 textures_coords[4];
    v4 color;
};

class TexturedQuadBuffer {
    unsigned int VAO, VBO, EBO, instanced_VBO;

	TexturedQuadBufferElement elements[MAX_CAPACITY];

public:
    std::string texture_key;
    u16 rolling_index = 0;

    void Allocate(u32 buffer_size, BufferLayout);
	void Flush();
	void Draw(Shader* shader, m4* projection);
	void Reset();
	
	u32 AddQuad(const v2 position, const v2 size, const v4 color, float rotation = 0);
    u32 AddTexturedQuad(const AtlasTextureInfo* texture_info, const Texture* atlas, const v2 pos, const v2 size = { 0, 0 },
        const float rotation = 0, v4 color = WHITE);

};

#endif
