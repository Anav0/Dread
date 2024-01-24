#ifndef Renderer_H
#define Renderer_H

#pragma once

#include "BoundingBox.h"
#include "Buffers.h"
#include "Constants.h"

#include <array>

class Texture;
class Shader;

typedef int ID;

class Model;
class MeshInBuffer;

class Renderer {

public:
    std::vector<BoundingBox> boxes;
    std::vector<InstancedMeshBuffer> buffers;
    TexturedQuadBuffer ui_buffer;

    m4 projection;
    m4 ortho_projection;

    Shader* object_shader;

    // Model index by bounding boxes
    std::map<std::string, u32> mesh_by_buffor_index;

    Renderer();
    void Draw();
    void Update();
    InstancedMeshBuffer* GetBuffer(std::string mesh_name);
    InstancedMeshBuffer* GetBufferByIndex(u32 index);
    InstancedMeshBuffer* CreateBuffer(Mesh mesh);
};

extern Renderer R;
#endif
