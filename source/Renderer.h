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

struct RenderGroup {
    ID id;
    MeshBuffer* buffer; // NOTE: buffer can be shared across several render groups
    Shader* shader;
    Texture* atlas;
    m4* projection;

    RenderGroup() { }

    RenderGroup(Shader* shader, m4* projection, MeshBuffer* buffer, Texture* atlas = nullptr);

    void Draw();
    void UpdateBufferSection(const int index, v2 size, v3 pos, float rotation = 0.0f);
    void UpdateColor(const int index, v4 color);
};

static constexpr int MAX_RENDER_GROUP = 4;

class Renderer {
    ID number_of_render_groups = 0;
    ID number_of_buffers = 0;

public:
    RenderGroup render_groups[MAX_RENDER_GROUP];
    std::vector<MeshBuffer> buffers;

    m4 projection;

    Shader* object_shader;

    // Model index by bounding boxes
    std::map<u32, std::vector<BoundingBox>> boxes;

    Renderer();
    int NumberOfRenderGroups();
    void AddRenderGroup(RenderGroup group);
    void Draw();
    void Update();
    MeshBuffer* CreateMeshBuffer();
};

extern Renderer R;
#endif
