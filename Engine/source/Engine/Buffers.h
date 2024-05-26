#ifndef Buffers_H
#define Buffers_H

#include "glm/ext/matrix_transform.hpp"
#pragma once

#include "GLM.h"
#include "Misc/Constants.h"

#include "Base.h"
#include "Mesh.h"
#include "Misc.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"

#include <string>
#include <array>

class Mesh;
class Model;
class MeshInBuffer;

static constexpr int MAX_CAPACITY = 512;


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

struct BufferInfo {
	u32 index;
	const std::string texture_key;
};

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

	void Enable() {

    u8 position = 3;
	for (BufferElement& el : elements) {
        GLenum gl_type = BufferElementTypeToOpenGLType(el.type);
        printf("%s\n", el.name.c_str());
        printf("glEnableVertexAttribArray(%i)\n", position);
        printf("glVertexAttribPointer(%i, %i, %i, GL_FALSE, %u, (void*)%u);\n", position, el.length, gl_type, size, el.offset);
        printf("glVertexAttribDivisor(%i, 1)\n", position);
        printf("\n");

        glEnableVertexAttribArray(position);
        if (el.IsInt()) {
            glVertexAttribIPointer(position, el.length, gl_type, size, (void*)el.offset);
        } else {
            glVertexAttribPointer(position, el.length, gl_type, GL_FALSE, size, (void*)el.offset);
        }

        glVertexAttribDivisor(position, 1);
        position++;
    }
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

struct MeshBufferElement {
	m4 matrice;
	v4 color;
	i32 id;
};

class InstancedMeshBuffer {
    u32 VBO = 0;

		std::vector<MeshBufferElement> elements;

public:
    Mesh mesh;
    InstancedMeshBuffer(Mesh mesh);

		void Draw(Shader* shader, v2 screen_size, m4& projection, m4& view, Texture* atlas);
		void Flush();
    void Allocate(u32 size, BufferLayout);

		void ScaleAll(f32 scale) {
			for(u32 i = 0; i < elements.size(); i++) {
				//elements.at(i).matrice = glm::scale(m4(1.0), v3(scale));
			}
		}

		m4 GetMatrix(const u32 index) {
			return elements.at(index).matrice;
		}

		void UpdateColor(const u32 index, v4 color) {
			elements.at(index).color = color;
		}

    MeshInBuffer AddMesh(v3 position, v3 size, v4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, i32 entity_id = -1, f32 rotation = 0.0f, f32 scale = 1.0f);
    MeshInBuffer AddMesh(v3 position, v4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, i32 entity_id = -1, f32 rotation = 0.0f, f32 scale = 1.0f);
};

constexpr u32 GRADIENT_MAX_COLORS = 4;
struct GradientBufferElement {
	v4 pos_and_size;
	m4 matrices;
	u32 gradient_type;
	v2 middle;
	f32 radial_factor;
	v4 radial_pos_and_smoothing;
  std::array<v4, GRADIENT_MAX_COLORS> colors;
};

enum class GradientType : i32 {
	Radial = 0,
	ThreeColor = 1,
};

struct Gradient {
	GradientType gradient_type;
	v2 middle;
	v2 radial_position;
	v2 smoothing;
	f32 radial_factor;
	std::array<v4, GRADIENT_MAX_COLORS> colors;
};

class GradientBuffer {
    unsigned int VAO, VBO, EBO, instanced_VBO;

	GradientBufferElement elements[MAX_CAPACITY];

public:
  u16 rolling_index = 0;

  void Allocate(u32 buffer_size, BufferLayout);
	void Flush();
	void Draw(v2 screen_size, Shader* shader, m4& projection);
	void Reset();
	
	u32 AddGradient(const v2 pos, const v2 size, const Gradient gradient);
};

struct TexturedQuadBufferElement {
    v2 pos;
    v2 size;
	m4 matrices;
	v2 textures_coords[4];
    v4 color;
};

typedef void (*pSetupShader) (Shader*, m4 projection);

class TexturedQuadBuffer {
  unsigned int VAO, VBO, EBO, instanced_VBO;

	TexturedQuadBufferElement elements[MAX_CAPACITY];

public:
  std::string texture_key;
  std::string shader_key;
  u16 rolling_index = 0;

  void Allocate(u32 buffer_size, BufferLayout);
	void Flush();
	void Draw(m4& projection);
	void Reset();
	
  u32 AddQuad(const v2 position, const v2 size, const v4 color, float rotation = 0);
  u32 AddTexturedQuad(const AtlasTextureInfo* texture_info, const Texture* atlas, const v2 pos, const v2 size = { 0, 0 }, const float rotation = 0, v4 color = WHITE);

};

class PickingBuffer {
  u32 FBO;
	u32 picking_texture;

	public:
		void Allocate(v2 screen_size);
		void Bind();
		void Unbind();
		f32 ReadPixel(v2 pos);
};

#endif
