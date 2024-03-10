#pragma once

#ifndef Renderer_H
#define Renderer_H

#include "BoundingBox.h"
#include "Buffers.h"
#include "Constants.h"

#include <array>

class Shader;
class Camera;

typedef int ID;

class Model;
class MeshInBuffer;

constexpr bool HIDE_ALPHA = true;

enum class RenderMode {
    WIREFRAME,
    NORMAL,
};

class Renderer {

public:
  std::vector<BoundingBox> boxes;
  std::vector<InstancedMeshBuffer> buffers;
  RenderMode mode = RenderMode::NORMAL;

	TexturedQuadBuffer font_buffer;
	TexturedQuadBuffer ui_buffer;
	GradientBuffer     gradient_buffer;

  m4 projection;
  m4 ortho_projection;

  // Model index by bounding boxes
  std::map<std::string, u32> mesh_by_buffor_index;

	void Init(Camera& camera, v2 screen_size);
  void Draw(Shader* shader, PickingBuffer*, Camera&, v2 screen_size);
  void DrawModels(Shader* shader, PickingBuffer*, Camera&, v2 screen_size);
	void ScaleAllModels(f32 scale);
	void Flush();
  void Update();
	void UpdateProjection(Camera& camera, v2 screen_size);
	void Reset();

  InstancedMeshBuffer* GetBuffer(const std::string& mesh_name);
  InstancedMeshBuffer* GetBufferByIndex(u32 index);
  InstancedMeshBuffer* CreateBuffer(Mesh mesh);
};

extern Renderer R;

#endif
