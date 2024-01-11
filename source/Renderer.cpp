#include "Renderer.h"
#include "Buffers.h"
#include "Model.h"

Renderer::Renderer()
{
}

void Renderer::Update()
{
    /* for (u16 i = 0; i < models.size(); i++) {
         auto model = &models.at(i);
         model->Update();
     }*/

    for (auto& [key, boxes] : R.boxes) {
        for (u16 i = 0; i < boxes.size(); i++) {
            auto box = &boxes.at(i);
            box->Update();
        }
    }
}

void Renderer::Draw()
{
    /* for (u16 i = 0; i < models.size(); i++) {
         auto model = models.at(i);
         model->Draw(this->object_shader, &projection);
     }*/

    for (auto& [key, boxes] : R.boxes) {
        for (u16 i = 0; i < boxes.size(); i++) {
            auto box = &boxes.at(i);
            box->Draw(&projection);
        }
    }
}

MeshBuffer* Renderer::CreateMeshBuffer()
{
    buffers[rolling_buffer_index] = MeshBuffer(rolling_buffer_index);

    rolling_buffer_index++;

    return &buffers[rolling_buffer_index - 1];
}

Renderer R;
