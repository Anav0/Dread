#include "Buffers.h"
#include "GameState.h"
#include "Mesh.h"
#include "Model.h"
#include "ResourceManager.h"
#include "TextRenderer.h"

InstancedMeshBuffer::InstancedMeshBuffer(Mesh mesh)
{
    this->mesh = mesh;
    glGenBuffers(1, &VBO);
}

void InstancedMeshBuffer::Allocate()
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

void InstancedMeshBuffer::Draw(Shader* shader, m4* projection, Texture* atlas)
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
    shader->setBool("hideAlpha", HIDE_ALPHA);
    shader->setMat4("projection", *projection);
    shader->setMat4("view", STATE.window.camera.GetViewMatrix());

    glBindVertexArray(mesh.VAO);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<u32>(mesh.indices.size()), GL_UNSIGNED_INT, 0, matrices.size());
    glBindVertexArray(0);
}

m4 InstancedMeshBuffer::GetMatrix(u32 index)
{
    return matrices[index];
}

MeshInBuffer InstancedMeshBuffer::AddMesh(v3 position, v3 size, v4 color, f32 rotation, f32 scale)
{
    auto mesh_in_buffer = MeshInBuffer();

    this->colors.push_back(color);
    this->matrices.push_back(GetTransformMatrix(position, size, rotation, v3(scale)));

    mesh_in_buffer.pos_in_buffer = this->colors.size() - 1;

    // Speed: slow and not ideal
    Allocate();

    return mesh_in_buffer;
}
MeshInBuffer InstancedMeshBuffer::AddMesh(v3 position, v4 color, f32 rotation, f32 scale)
{
    auto mesh_in_buffer = MeshInBuffer();

    this->colors.push_back(color);
    this->matrices.push_back(GetTransformMatrix(position, rotation, v3(scale)));

    mesh_in_buffer.pos_in_buffer = this->colors.size() - 1;

    // Speed: slow and not ideal
    Allocate();

    return mesh_in_buffer;
}

u32 TexturedQuadBuffer::AddTexturedRect(const AtlasTextureInfo* texture_info, const Texture* atlas, const v2 pos, const v2 size, const float rotation, v4 color)
{
    assert(rolling_index >= 0);
    assert(rolling_index <= MAX_CAPACITY);

    v2 size_to_use = size;

    if (size.x == 0 && size.y == 0)
        size_to_use = texture_info->size;

    this->colors[rolling_index] = color;
    this->matrices[rolling_index] = GetTransformMatrix(pos, size_to_use, rotation, texture_info->scale);

    float subtex_w = texture_info->size.x / atlas->Width;
    float subtex_h = texture_info->size.y / atlas->Height;
    float subtex_x = texture_info->position.x / atlas->Width;
    float subtex_y = texture_info->position.y / atlas->Height;

    int coords_index = rolling_index * 4;

    textures_coords[coords_index] = { subtex_x + subtex_w, subtex_y + subtex_h }; // TR
    textures_coords[coords_index + 1] = { subtex_x + subtex_w, subtex_y }; // BR
    textures_coords[coords_index + 2] = { subtex_x, subtex_y }; // BL
    textures_coords[coords_index + 3] = { subtex_x, subtex_y + subtex_h }; // TL

    UpdateBufferSection(rolling_index);

    const int tmp = rolling_index;
    rolling_index += 1;
    return tmp;
}

u32 TexturedQuadBuffer::AddRect(const Rectangle rect)
{
    assert(rolling_index >= 0);
    assert(rolling_index <= MAX_CAPACITY);

    this->colors[rolling_index] = rect.color;
    this->matrices[rolling_index] = GetTransformMatrix(rect.transform.position, rect.transform.size, rect.transform.rotation);

    int coords_index = rolling_index * 4;

    textures_coords[coords_index] = { -1, -1 };
    textures_coords[coords_index + 1] = { -1, -1 };
    textures_coords[coords_index + 2] = { -1, -1 };
    textures_coords[coords_index + 3] = { -1, -1 };

    UpdateBufferSection(rolling_index);

    const int tmp = rolling_index;
    rolling_index += 1;
    return tmp;
}

void TexturedQuadBuffer::Allocate()
{
    constexpr float vertices[] = {
        0.5f, 0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f // top left
    };

    constexpr unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    // Gen
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanced_VBO);

    // Bind
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(matrices) + sizeof(textures_coords) + sizeof(colors), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(matrices), &matrices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(matrices), sizeof(textures_coords), &textures_coords[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(matrices) + sizeof(textures_coords), sizeof(colors), &colors[0]);

    // vec3 position
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
    glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)sizeof(matrices));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(matrices) + 2 * sizeof(float)));
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(matrices) + 4 * sizeof(float)));
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(matrices) + 6 * sizeof(float)));
    glEnableVertexAttribArray(11);
    glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(matrices) + sizeof(textures_coords)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glVertexAttribDivisor(7, 1); // Tex coords
    glVertexAttribDivisor(8, 1); // Tex coords
    glVertexAttribDivisor(9, 1); // Tex coords
    glVertexAttribDivisor(10, 1); // Tex coords
    glVertexAttribDivisor(11, 1); // color

    glBindVertexArray(0);
}

u16 TexturedQuadBuffer::GetCurrentIndex()
{
    return rolling_index;
}
