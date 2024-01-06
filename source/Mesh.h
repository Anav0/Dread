#ifndef MESH_H
#define MESH_H

#pragma once

#include <string>
#include <vector>

#include "Base.h"
#include "Constants.h"
#include "Texture.h"

using std::string;
using std::vector;

class Shader;

class MeshInBuffer {
public:
    u32 mesh_data_index; // Index under which this mesh instance is present in MeshBuffer
    u32 color_data_index; // Index under which this mesh's color  is present in MeshBuffer

    void ChangeColor(v4 color);
    void ChangePosition(v3 position);
    void ChangeSize(v3 size);
};

struct ModelInBuffer {
    std::vector<MeshInBuffer> meshes;
};

class Mesh {
public:
    std::string id;
    unsigned int VAO;
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<std::string> textures;
    u64 triangle_count;

    Mesh() { }

    Mesh(u64 triangle_count, vector<Vertex> vertices, vector<unsigned int> indices, vector<std::string> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->triangle_count = triangle_count;

        Setup();
    }

    void Update();
    void Draw(Shader* shader, m4* projection, m4 model);
    void UpdateBuffer();

private:
    unsigned int VBO, EBO;

    // TODO: make Mesh just be a holder of data, and
    // use render groups to render any number of meshes
    void Setup();
};

#endif
