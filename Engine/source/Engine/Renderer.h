#pragma once

#ifndef Renderer_H
#define Renderer_H

#include "BoundingBox.h"
#include "Particles.h"
#include "Buffers.h"
#include "Constants.h"

#include <array>

class Shader;
class Camera;

typedef int ID;

class Model;
class MeshInBuffer;

constexpr bool HIDE_ALPHA = true;

constexpr u32 UI_BUFFER_INDEX   = 0;
constexpr u32 FONT_BUFFER_INDEX = 1;
constexpr u32 BEAM_BUFFER_INDEX = 2;

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
  void DrawModels(Shader* shader, PickingBuffer*, Camera&, v2 screen_size);
	void ScaleAllModels(f32 scale);
	void Flush();
  void Update(f32 dt);
	void UpdateProjection(Camera& camera, v2 screen_size);
	void Reset();

  InstancedMeshBuffer* GetBuffer(const std::string& mesh_name);
  InstancedMeshBuffer* GetBufferByIndex(u32 index);
  InstancedMeshBuffer* CreateBuffer(Mesh mesh);

	u32 CreateQuadBuffer(std::string shader_key, std::string texture_key);
	TexturedQuadBuffer& GetQuadBuffer(u32 index);
};

extern Renderer R;

#endif
