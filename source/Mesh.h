#ifndef MESH_H
#define MESH_H

#pragma once

#include <string>
#include <vector>

#include "Base.h"
#include "Texture.h"

using std::string;
using std::vector;

class Shader;

class Mesh {
public:
    unsigned int VAO;
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<std::string>  textures;
    m4 model = glm::translate(m4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));

    Mesh() { }

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<std::string> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        SetupBoundingBox();
        Setup();
    }

    void Update();
    void Draw(Shader *shader, m4 *projection);
    void UpdateBuffer();
    
private:
    unsigned int VBO, EBO;

    // TODO: make Mesh just be a holder of data, and
    // use render groups to render any number of meshes
    void Setup();
    void CalculateBoundingBoxSizeAndCenter(v3 *size, v3 *center);
    void SetupBoundingBox();
    
};

#endif
