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

class Renderer {
    ID rolling_buffer_index = 0;

public:
    std::vector<MeshBuffer> buffers;

    m4 projection;

    Shader* object_shader;

    // Model index by bounding boxes
    std::map<u32, std::vector<BoundingBox>> boxes;

    Renderer();
    int NumberOfRenderGroups();
    void Draw();
    void Update();
    MeshBuffer* CreateMeshBuffer();
};

extern Renderer R;
#endif
