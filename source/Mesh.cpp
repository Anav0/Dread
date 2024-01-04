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

void Mesh::Draw(Shader* shader, m4* projection)
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
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
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

// TODO: make Mesh just be a holder of data, and
// use render groups to render any number of meshes
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

void Mesh::CalculateBoundingBoxSizeAndCenter(v3* size, v3* center)
{
    f32 max_x = vertices[0].Position.x;
    f32 max_y = vertices[0].Position.y;
    f32 max_z = vertices[0].Position.z;

    f32 min_x = vertices[0].Position.x;
    f32 min_y = vertices[0].Position.y;
    f32 min_z = vertices[0].Position.z;

    for (size_t i = 0; i < vertices.size(); i++) {
        Vertex* v = &vertices[i];

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

void Mesh::SetupBoundingBox()
{
    v3 size, center;
    CalculateBoundingBoxSizeAndCenter(&size, &center);

    R.boxes.push_back(BoundingBox(size, center));
}
