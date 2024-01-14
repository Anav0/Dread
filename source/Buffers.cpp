#include "Buffers.h"
#include "Mesh.h"
#include "Model.h"
#include "ResourceManager.h"
#include "GameState.h"

InstancedBuffer::InstancedBuffer(Mesh mesh)
{
    this->mesh = mesh;
    glGenBuffers(1, &VBO);
}

void InstancedBuffer::Allocate()
{
    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    auto matrices_size = sizeof(m4) * matrices.size();
    auto colors_size = sizeof(v4) * colors.size();

    std::size_t vec4Size = sizeof(v4);

    auto stride = 4 * sizeof(v4);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * vec4Size));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(v4), (void*)matrices_size);

    glVertexAttribDivisor(3, 1); // Matrix transform
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1); // Color

    glBufferData(GL_ARRAY_BUFFER, matrices_size + colors_size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, matrices_size, &matrices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, matrices_size, colors_size, &colors[0]);

    glBindVertexArray(0);
}

void InstancedBuffer::Draw(Shader* shader, m4* projection, Texture* atlas)
{
    if (colors.size() == 0)
        return;

    if (atlas != nullptr) {
        glActiveTexture(GL_TEXTURE0);
        atlas->Bind();
    }

    if (STATE.mode == RenderMode::WIREFRAME)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    shader->Use();
    shader->setInt("imageSampler", 0);
    shader->setMat4("projection", *projection);
    shader->setMat4("view", STATE.window.camera.GetViewMatrix());

    glBindVertexArray(mesh.VAO);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<u32>(mesh.indices.size()), GL_UNSIGNED_INT, 0, matrices.size());
    glBindVertexArray(0);
}

m4 InstancedBuffer::GetMatrix(u32 index)
{
    return matrices[index];
}

MeshInBuffer InstancedBuffer::AddMesh(v3 position, v3 size, v4 color, f32 rotation, f32 scale)
{
    auto mesh_in_buffer = MeshInBuffer();

    this->colors.push_back(color);
    this->matrices.push_back(GetTransformMatrix(position, size, rotation, v3(scale)));

    mesh_in_buffer.pos_in_buffer = this->colors.size() - 1;

    // Speed: slow and not ideal
    Allocate();

    return mesh_in_buffer;
}
MeshInBuffer InstancedBuffer::AddMesh(v3 position, v4 color, f32 rotation, f32 scale)
{
    auto mesh_in_buffer = MeshInBuffer();

    this->colors.push_back(color);
    this->matrices.push_back(GetTransformMatrix(position, rotation, v3(scale)));

    mesh_in_buffer.pos_in_buffer = this->colors.size() - 1;

    // Speed: slow and not ideal
    Allocate();

    return mesh_in_buffer;
}