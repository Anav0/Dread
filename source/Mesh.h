#ifndef MESH_H
#define MESH_H

#pragma once

#include "Base.h"
#include "Constants.h"
#include "Shader.h"
#include "Texture.h"

#include "BoundingBox.h"
#include "EntityManager.h"
#include "RenderHelpers.h"
#include "Renderer.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

class Mesh {
public:
    unsigned int VAO;
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    ID bounding_box_id; // TODO: take it out since Mesh may be reused
    m4 model = glm::translate(m4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));

    Mesh() { }

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        SetupBoundingBox();
        Setup();
    }

    void Update()
    {
        // m4* model = &R.transformations[model_id];
        // model = glm::translate(model, v3(0.0, 0.05, 0.0));
        // BoundingBox* box = &R.boxes.at(bounding_box_id);
        // box->is_dirty = true;
    }

    void Draw(Shader *shader, m4 *projection)
    {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;

        if (STATE.mode == RenderMode::WIREFRAME)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        shader->setMat4("projection", *projection);
        shader->setMat4("view", STATE.window.camera.GetViewMatrix());
        shader->setMat4("model", model);

        for (unsigned int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);
            else if (name == "texture_height")
                number = std::to_string(heightNr++);

            glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].ID);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

    void UpdateBuffer()
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

private:
    unsigned int VBO, EBO;

    // TODO: make Mesh just be a holder of data, and
    // use render groups to render any number of meshes
    void Setup()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        // texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        // tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

        // bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

        // color
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

        glBindVertexArray(0);
    }

    void CalculateBoundingBoxSizeAndCenter(v3 *size, v3 *center)
    {
        f32 max_x = 0, max_y = 0, max_z = 0.0;
        f32 min_x = 0, min_y = 0, min_z = 0;

        for (size_t i = 0; i < vertices.size(); i++)
        {
            Vertex *v = &vertices[i];

            if (v->Position.x < min_x)
                min_x = v->Position.x;
            if (v->Position.y < min_y)
                min_y = v->Position.y;
            if (v->Position.z < min_z)
                min_z = v->Position.z;

            if (v->Position.x > max_x)
                max_x = v->Position.x;
            if (v->Position.y > max_y)
                max_y = v->Position.y;
            if (v->Position.z > max_z)
                max_z = v->Position.z;
        }

        *size = v3(max_x - min_x, max_y - min_y, max_z - min_z);
        *center = v3((min_x + max_x) / 2.0f, (min_y + max_y) / 2.0f, (min_z + max_z) / 2.0f);
    }
    void SetupBoundingBox()
    {
        v3 size, center;
        CalculateBoundingBoxSizeAndCenter(&size, &center);

        R.boxes.push_back(BoundingBox(size, center));
        bounding_box_id = R.boxes.size() - 1;
    }
};

#endif
