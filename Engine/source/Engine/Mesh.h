#ifndef MESH_H
#define MESH_H

#pragma once

#include <string>
#include <vector>

#include "Base.h"
#include "GLM.h"
#include "Misc/Constants.h"
#include "Texture.h"

using std::string;
using std::vector;

class Shader;
class PickingBuffer;

class MeshInBuffer {
public:
    u32 pos_in_buffer;
    u32 buffer_index;
    m4 GetMatrix();
    void ChangeColor(v4 color);
    void ChangePosition(v3 position);
    void ChangeSize(v3 size);
};

class Mesh {
public:
    u32 VAO;
    std::string id;
    vector<Vertex> vertices;
    vector<u32> indices;
    vector<std::string> textures;
    u64 triangle_count;

    Mesh() { }

    Mesh(u64 triangle_count, vector<Vertex> vertices, vector<u32> indices, vector<std::string> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->triangle_count = triangle_count;

        Setup();
    }

private:
    u32 VBO, EBO;

    // TODO: make Mesh just be a holder of data, and
    // use render groups to render any number of meshes
    void Setup();
};

#endif
