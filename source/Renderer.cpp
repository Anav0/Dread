#include "Renderer.h"
#include "Buffers.h"

void RenderGroup::Draw()
{
    buffer->Draw(shader, projection, atlas);
}

void RenderGroup::UpdateBufferSection(const int index, v2 size, v3 pos, float rotation)
{
    buffer->UpdateMatrix(index, size, pos, rotation);
    buffer->UpdateBufferSection(index);
}

RenderGroup::RenderGroup(Shader* shader, m4* projection, VertexBuffer* buffer, Texture* atlas)
{
    this->buffer = buffer;
    this->projection = projection;
    this->shader = shader;
    this->atlas = atlas;
}

void RenderGroup::UpdateColor(const int index, v4 color)
{
    buffer->UpdateColor(index, color);
    buffer->UpdateBufferSection(index);
}

int Renderer::NumberOfRenderGroups()
{
    return number_of_render_groups;
}

Renderer::Renderer() { }

void Renderer::AddRenderGroup(RenderGroup group)
{
    group.id = number_of_render_groups++;
    render_groups[group.id] = group;
}

void Renderer::Draw()
{
    for (size_t i = 0; i < number_of_render_groups; i++) {
        render_groups[i].Draw();
    }
}

VertexBuffer* Renderer::CreateBuffer()
{
    buffers[number_of_buffers++] = VertexBuffer(number_of_buffers + 1);
    return &buffers[number_of_buffers - 1];
}

Renderer R;
