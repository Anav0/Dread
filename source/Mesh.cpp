#include "Mesh.h"
#include "Constants.h"
#include "Shader.h"

#include "EntityManager.h"
#include "RenderHelpers.h"
#include "Renderer.h"

void Mesh::Update()
{
    // m4* model = &R.transformations[model_id];
    // model = glm::translate(model, v3(0.0, 0.05, 0.0));
    // BoundingBox* box = &R.boxes.at(bounding_box_id);
    // box->is_dirty = true;
}

void Mesh::Draw(Shader* shader, m4* projection, m4 model)
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
        Texture* t = RM.GetTextureByKey(textures[i]);
        if (t->type == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (t->type == "texture_specular")
            number = std::to_string(specularNr++);
        else if (t->type == "texture_normal")
            number = std::to_string(normalNr++);
        else if (t->type == "texture_height")
            number = std::to_string(heightNr++);

        glUniform1i(glGetUniformLocation(shader->ID, (t->type + number).c_str()), i);
        glBindTexture(GL_TEXTURE_2D, t->ID);
    }

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, 10);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
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

    glBindVertexArray(0);
}

std::string MeshInBuffer::DeriveKeyFromIndexes()
{
    return std::string(buffer_index + "_" + pos_in_buffer);
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
