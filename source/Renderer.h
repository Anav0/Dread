#ifndef Renderer_H
#define Renderer_H

#pragma once

#include "Buffers.h"
#include "Constants.h"
//#include "Model.h"

class Texture;
class Shader;

typedef int ID;

class Model;

struct RenderGroup {
    ID id;
    VertexBuffer* buffer; // NOTE: buffer can be shared across several render groups
    Shader* shader;
    Texture* atlas;
    m4* projection;

    RenderGroup() { }

    RenderGroup(Shader* shader, m4* projection, VertexBuffer* buffer, Texture* atlas = nullptr);

    void Draw();
    void UpdateBufferSection(const int index, v2 size, v3 pos, float rotation = 0.0f);
    void UpdateColor(const int index, v4 color);
};

static constexpr int MAX_RENDER_GROUP = 4;
static constexpr int MAX_BUFFERS = MAX_RENDER_GROUP * 2; // Maybe use vector

class Renderer {
    ID number_of_render_groups = 0;
    ID number_of_buffers = 0;

public:
    RenderGroup render_groups[MAX_RENDER_GROUP];
    VertexBuffer buffers[MAX_BUFFERS];
    m4 projection;

    // TODO: for now models are not using render groups
    Shader* object_shader;
    std::vector<Model> models;

    Renderer();
    int NumberOfRenderGroups();
    void AddRenderGroup(RenderGroup group);
    void Draw();
    VertexBuffer* CreateBuffer();
};

extern Renderer R;
#endif
