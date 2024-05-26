#pragma once

#ifndef Renderer_H
#define Renderer_H

#include "BoundingBox.h"
#include "Particles.h"
#include "Buffers.h"
#include "GLM.h"
#include "Misc/Constants.h"

#include <array>

class Shader;
class Camera;

typedef int ID;

class Model;
class MeshInBuffer;

constexpr bool HIDE_ALPHA = true;

constexpr u32 UI_BUFFER_INDEX       = 0;
constexpr u32 INSIGNIA_BUFFER_INDEX = 1;
constexpr u32 FONT_BUFFER_INDEX     = 2;

static BufferInfo UI_BUFFER_INFO {
	UI_BUFFER_INDEX,
	"icons",
};

static BufferInfo INSIGNIA_BUFFER_INFO {
	INSIGNIA_BUFFER_INDEX,
	"insignia",
};

enum class RenderMode {
    WIREFRAME,
    NORMAL,
};

class Renderer {

public:
  std::vector<BoundingBox> boxes;
  std::vector<InstancedMeshBuffer> buffers;
  RenderMode mode = RenderMode::NORMAL;

	std::vector<TexturedQuadBuffer> quad_buffers;

	GradientBuffer gradient_buffer;

	std::vector<ParticlesEmitter> emitters;

  m4 projection;
  m4 ortho_projection;

  // Model index by bounding boxes
  std::map<std::string, u32> mesh_by_buffor_index;

	void Init(Camera& camera, v2 screen_size);
  void Draw(Camera&, v2 screen_size);
  void DrawModels(Shader* shader, Camera&, v2 screen_size);
	void ScaleAllModels(f32 scale);
	void Flush();
	void FlushModels();
  void Update(f32 dt);
	void UpdateProjection(Camera& camera, v2 screen_size);
	void Reset();

  InstancedMeshBuffer* GetBuffer(const std::string& mesh_name);
  InstancedMeshBuffer* GetBufferByIndex(u32 index);
  InstancedMeshBuffer* CreateBuffer(u32 size, Mesh mesh);

	u32 CreateQuadBuffer(u32 size, std::string shader_key, std::string texture_key);
	TexturedQuadBuffer& GetQuadBuffer(u32 index);
};

extern Renderer R;

#endif
