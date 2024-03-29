#include "Buffers.h"
#include "Mesh.h"
#include "Model.h"
#include "ResourceManager.h"
#include "TextRenderer.h"
#include "glad/glad.h"

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
    auto ids_size = sizeof(i32) * ids.size();

    auto total_size = matrices_size + colors_size + ids_size;

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
    glEnableVertexAttribArray(8);
    glVertexAttribIPointer(8, 1, GL_INT, sizeof(i32), (void*)(matrices_size + colors_size));

    glVertexAttribDivisor(3, 1); // Matrix transform
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1); // Color
    glVertexAttribDivisor(8, 1); // EntityId

    glBufferData(GL_ARRAY_BUFFER, total_size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, matrices_size, &matrices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, matrices_size, colors_size, &colors[0]);
    glBufferSubData(GL_ARRAY_BUFFER, matrices_size + colors_size, ids_size, &ids[0]);

    glBindVertexArray(0);
}

void InstancedMeshBuffer::Draw(Shader* shader, v2 screen_size, PickingBuffer* picking_buffer, m4& projection, m4& view, Texture* atlas)
{
    if (colors.size() == 0)
        return;

    if (atlas != nullptr) {
        glActiveTexture(GL_TEXTURE0);
        atlas->Bind();
    }

    glEnable(GL_BLEND);
    glViewport(0, 0, screen_size.x, screen_size.y);

    if (R.mode == RenderMode::WIREFRAME)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    shader->Use();
    shader->setInt("imageSampler", 0);
    shader->setBool("hideAlpha", HIDE_ALPHA);
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);

    glBindVertexArray(mesh.VAO);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<u32>(mesh.indices.size()), GL_UNSIGNED_INT, 0, matrices.size());
    glBindVertexArray(0);
}

m4 InstancedMeshBuffer::GetMatrix(u32 index)
{
    return matrices[index];
}

MeshInBuffer InstancedMeshBuffer::AddMesh(v3 position, v3 size, v4 color, i32 entity_id, f32 rotation, f32 scale)
{
    auto mesh_in_buffer = MeshInBuffer();

    this->colors.push_back(color);
    this->matrices.push_back(GetTransformMatrix(position, size, rotation, v3(scale)));
    this->ids.push_back(entity_id);

    mesh_in_buffer.pos_in_buffer = this->colors.size() - 1;

    // Speed: slow and not ideal
    Allocate();

    return mesh_in_buffer;
}
MeshInBuffer InstancedMeshBuffer::AddMesh(v3 position, v4 color, i32 entity_id, f32 rotation, f32 scale)
{
    auto mesh_in_buffer = MeshInBuffer();

    this->colors.push_back(color);
    this->matrices.push_back(GetTransformMatrix(position, rotation, v3(scale)));
    this->ids.push_back(entity_id);

    mesh_in_buffer.pos_in_buffer = this->colors.size() - 1;

    // Speed: slow and not ideal
    Allocate();

    return mesh_in_buffer;
}

u32 TexturedQuadBuffer::AddTexturedQuad(const AtlasTextureInfo* texture_info, const Texture* atlas, const v2 pos, const v2 size, const float rotation, v4 color)
{
    assert(rolling_index >= 0);
    assert(rolling_index + 1 <= MAX_CAPACITY);

    TexturedQuadBufferElement el {};

    v2 size_to_use = size;

    if (size.x == 0 && size.y == 0)
        size_to_use = texture_info->size;

    el.size = size_to_use;
    el.color = color;
    el.matrices = GetTransformMatrix(pos, size_to_use, rotation, texture_info->scale);

    float subtex_w = texture_info->size.x / atlas->Width;
    float subtex_h = texture_info->size.y / atlas->Height;
    float subtex_x = texture_info->position.x / atlas->Width;
    float subtex_y = texture_info->position.y / atlas->Height;

    el.textures_coords[0] = { subtex_x + subtex_w, subtex_y + subtex_h }; // TR
    el.textures_coords[1] = { subtex_x + subtex_w, subtex_y }; // BR
    el.textures_coords[2] = { subtex_x, subtex_y }; // BL
    el.textures_coords[3] = { subtex_x, subtex_y + subtex_h }; // TL

    elements[rolling_index] = el;

    const int tmp = rolling_index;
    rolling_index += 1;
    return tmp;
}

u32 TexturedQuadBuffer::AddQuad(const v2 position, const v2 size, const v4 color, float rotation)
{
    assert(rolling_index >= 0);
    assert(rolling_index + 1 <= MAX_CAPACITY);

    TexturedQuadBufferElement el {};

    el.size = size;
    el.color = color;
    el.matrices = GetTransformMatrix(position, size, rotation);

    el.textures_coords[0] = { -1, -1 };
    el.textures_coords[1] = { -1, -1 };
    el.textures_coords[2] = { -1, -1 };
    el.textures_coords[3] = { -1, -1 };

    elements[rolling_index] = el;

    const int tmp = rolling_index;
    rolling_index += 1;
    return tmp;
}

GLenum BufferElementTypeToOpenGLType(BufferElementType type)
{
    switch (type) {
    case BufferElementType::Float:
        return GL_FLOAT;
    case BufferElementType::VFloat2:
        return GL_FLOAT;
    case BufferElementType::VFloat3:
        return GL_FLOAT;
    case BufferElementType::VFloat4:
        return GL_FLOAT;
    case BufferElementType::Int:
        return GL_INT;
    case BufferElementType::UInt:
        return GL_UNSIGNED_INT;
    case BufferElementType::VUInt2:
        return GL_UNSIGNED_INT;
    case BufferElementType::VUInt3:
        return GL_UNSIGNED_INT;
    case BufferElementType::VUInt4:
        return GL_UNSIGNED_INT;
    case BufferElementType::VInt2:
        return GL_INT;
    case BufferElementType::VInt3:
        return GL_INT;
    case BufferElementType::VInt4:
        return GL_INT;
    case BufferElementType::Bool:
        return GL_BOOL;
    }
    return 0;
}

u8 GetBufferElementSize(BufferElementType type)
{
    switch (type) {
    case BufferElementType::Bool:
        return 1;
    case BufferElementType::VBool2:
        return 2;
    case BufferElementType::VBool3:
        return 3;
    case BufferElementType::VBool4:
        return 4;

    case BufferElementType::Float:
    case BufferElementType::Int:
        return 4;

    case BufferElementType::VFloat2:
    case BufferElementType::VUInt2:
    case BufferElementType::VInt2:
        return 4 * 2;

    case BufferElementType::VFloat3:
    case BufferElementType::VUInt3:
    case BufferElementType::VInt3:
        return 4 * 3;

    case BufferElementType::VFloat4:
    case BufferElementType::VUInt4:
    case BufferElementType::VInt4:
        return 4 * 4;
    }
}

u8 GetBufferElementTypeLength(BufferElementType type)
{
    switch (type) {
    case BufferElementType::Bool:
        return 1;
    case BufferElementType::VBool2:
        return 2;
    case BufferElementType::VBool3:
        return 3;
    case BufferElementType::VBool4:
        return 4;

    case BufferElementType::Int:
        return 1;
    case BufferElementType::VInt2:
        return 2;
    case BufferElementType::VInt3:
        return 3;
    case BufferElementType::VInt4:
        return 4;

    case BufferElementType::Float:
        return 1;
    case BufferElementType::VFloat2:
        return 2;
    case BufferElementType::VFloat3:
        return 3;
    case BufferElementType::VFloat4:
        return 4;
    }
}

//------------------------------------------------------------------------

void TexturedQuadBuffer::Allocate(u32 buffer_size, BufferLayout layout)
{
    constexpr f32 vertices[] = {
        0.5f, 0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f // top left
    };

    constexpr u32 indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanced_VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);
    glBufferData(GL_ARRAY_BUFFER, buffer_size * sizeof(TexturedQuadBufferElement), NULL, GL_DYNAMIC_DRAW);

    u8 position = 3;
    for (BufferElement& el : layout.elements) {
        GLenum gl_type = BufferElementTypeToOpenGLType(el.type);
        printf("glEnableVertexAttribArray(%i)\n", position);
        printf("glVertexAttribPointer(%i, %i, %i, GL_FALSE, %u, (void*)%u);\n", position, el.length, gl_type, layout.size, el.offset);
        printf("glVertexAttribDivisor(%i, 1)\n", position);
        printf("\n");

        glEnableVertexAttribArray(position);
        if (el.IsInt()) {
            glVertexAttribIPointer(position, el.length, gl_type, layout.size, (void*)el.offset);
        } else {
            glVertexAttribPointer(position, el.length, gl_type, GL_FALSE, layout.size, (void*)el.offset);
        }

        glVertexAttribDivisor(position, 1);
        position++;
    }

    glBindVertexArray(0);
}

void TexturedQuadBuffer::Flush()
{
    if (rolling_index == 0)
        return;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);

    u64 size = rolling_index * sizeof(TexturedQuadBufferElement);

    assert(size > 0);

    glBufferSubData(GL_ARRAY_BUFFER, 0, size, &elements[0]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TexturedQuadBuffer::Draw(m4& projection)
{
    if (rolling_index == 0)
        return;

    auto shader = RM.GetShader(this->shader_key);

    switch (shader->type) {
    case ShaderType::TexturedQuad:
        UseTextureShader(shader, projection);
        break;
    }

    if (this->texture_key != "") {
        auto atlas = RM.GetTexture(this->texture_key);
        glActiveTexture(GL_TEXTURE0);
        atlas->Bind();
    }

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, rolling_index);
    glBindVertexArray(0);
}

void TexturedQuadBuffer::Reset()
{
    // This should be enough
    rolling_index = 0;
}

u32 GradientBuffer::AddGradient(const v2 pos, const v2 size, const Gradient gradient)
{
    assert(rolling_index >= 0);
    assert(rolling_index + 1 <= MAX_CAPACITY);

    GradientBufferElement el {};

		el.pos_and_size = { pos, size };
    el.matrices = GetTransformMatrix(pos, size, 0);
    el.colors = gradient.colors;
    el.gradient_type = static_cast<u32>(gradient.gradient_type);
    el.middle = gradient.middle;
    el.radial_pos_and_smoothing = { gradient.radial_position, gradient.smoothing };
    el.radial_factor = gradient.radial_factor;

    elements[rolling_index] = el;

    const int tmp = rolling_index;
    rolling_index += 1;
    return tmp;
}

void GradientBuffer::Allocate(u32 buffer_size, BufferLayout layout)
{
    constexpr f32 vertices[] = {
        0.5f, 0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f // top left
    };

    constexpr u32 indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanced_VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);
    glBufferData(GL_ARRAY_BUFFER, buffer_size * sizeof(GradientBufferElement), NULL, GL_DYNAMIC_DRAW);

    u8 position = 3;
		printf("GRADIENT LAYOUT\n");
		printf("====================================\n");
    for (BufferElement& el : layout.elements) {
        GLenum gl_type = BufferElementTypeToOpenGLType(el.type);
        printf("glEnableVertexAttribArray(%i)\n", position);
        printf("glVertexAttribPointer(%i, %i, %i, GL_FALSE, %u, (void*)%u);\n", position, el.length, gl_type, layout.size, el.offset);
        printf("glVertexAttribDivisor(%i, 1)\n", position);
        printf("\n");

        glEnableVertexAttribArray(position);
        if (el.IsInt()) {
            glVertexAttribIPointer(position, el.length, gl_type, layout.size, (void*)el.offset);
        } else {
            glVertexAttribPointer(position, el.length, gl_type, GL_FALSE, layout.size, (void*)el.offset);
        }
        glVertexAttribDivisor(position, 1);
        position++;
    }
		printf("====================================\n");

    glBindVertexArray(0);
}

void GradientBuffer::Flush()
{
    if (rolling_index == 0)
        return;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanced_VBO);

    u64 size = rolling_index * sizeof(GradientBufferElement);

    assert(size > 0);

    glBufferSubData(GL_ARRAY_BUFFER, 0, size, &elements[0]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GradientBuffer::Draw(v2 screen_size, Shader* shader, m4& projection)
{
    if (rolling_index == 0)
        return;

    shader->Use();
    shader->setMat4("projection", projection);
    shader->setVec2("resolution", screen_size);

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, rolling_index);
    glBindVertexArray(0);
}

void GradientBuffer::Reset()
{
    // This should be enough
    rolling_index = 0;
}

void PickingBuffer::Allocate(v2 screen_size)
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &picking_texture);
    glBindTexture(GL_TEXTURE_2D, picking_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screen_size.x, screen_size.y, 0, GL_RGB, GL_FLOAT, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, picking_texture, 0);

    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PickingBuffer::Bind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glBindTexture(GL_TEXTURE_2D, picking_texture);
}

void PickingBuffer::Unbind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

f32 PickingBuffer::ReadPixel(v2 pos)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBindTexture(GL_TEXTURE_2D, picking_texture);

    f32 pixel[3];

    glReadPixels(pos.x, pos.y, 1, 1, GL_RGB, GL_FLOAT, &pixel);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return pixel[0];
}
