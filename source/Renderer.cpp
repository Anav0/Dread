#include "Renderer.h"
#include "Buffers.h"
#include "Model.h"

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

Renderer::Renderer()
{
}

void Renderer::AddRenderGroup(RenderGroup group)
{
    group.id = number_of_render_groups++;
    render_groups[group.id] = group;
}

void Renderer::Update()
{
    /* for (u16 i = 0; i < models.size(); i++) {
         auto model = &models.at(i);
         model->Update();
     }*/

    for (u16 i = 0; i < boxes.size(); i++) {
        auto box = &boxes.at(i);
        box->Update();
    }
}

void Renderer::Draw()
{
    for (u16 i = 0; i < number_of_render_groups; i++) {
        render_groups[i].Draw();
    }

    for (u16 i = 0; i < models.size(); i++) {
        auto model = models.at(i);
        model->Draw(this->object_shader, &projection);
    }

    if (STATE.bounding_draw_mode == BoundingDrawMode::SET) {
        for (auto index : STATE.bounding_boxes_to_draw) {
            auto box = &boxes[index];
            box->Draw(this->object_shader, &projection);
        }
    } else {
        for (u16 i = 0; i < boxes.size(); i++) {
            auto box = &boxes.at(i);
            box->Draw(this->object_shader, &projection);
        }
    }
}

VertexBuffer* Renderer::CreateBuffer()
{
    buffers[number_of_buffers++] = VertexBuffer(number_of_buffers + 1);
    return &buffers[number_of_buffers - 1];
}

Renderer R;
