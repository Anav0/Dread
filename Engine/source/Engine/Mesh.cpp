#include "Mesh.h"
#include "Constants.h"
#include "Shader.h"
#include "Buffers.h"

#include "Renderer.h"

void Mesh::Update()
{
    // m4* model = &R.transformations[model_id];
    // model = glm::translate(model, v3(0.0, 0.05, 0.0));
    // BoundingBox* box = &R.boxes.at(bounding_box_id);
    // box->is_dirty = true;
}

void Mesh::UpdateBuffer()
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void Mesh::Setup()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), &indices[0], GL_STATIC_DRAW);

    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

m4 MeshInBuffer::GetMatrix()
{
    auto buffer = &R.buffers[buffer_index];
    return buffer->GetMatrix(pos_in_buffer);
}

void MeshInBuffer::ChangeColor(v4 color)
{
    auto buffer = R.GetBufferByIndex(buffer_index);
    buffer->UpdateColor(pos_in_buffer, color);
}
