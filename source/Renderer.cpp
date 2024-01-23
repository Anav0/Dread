#include "Renderer.h"
#include "Buffers.h"
#include "Model.h"

Renderer::Renderer()
{
}

void Renderer::Update()
{
}

void Renderer::Draw()
{
    // TODO: temp
    auto shader = RM.GetShader("object");
    auto texture_shader = RM.GetShader("texture");

    ui_buffer.Draw(texture_shader, &ortho_projection);

    for (auto& buffer : buffers) {
        buffer.Draw(shader, &projection, nullptr);
    }

    if (STATE.show_collisions) {
        for (auto& box : R.boxes) {
            box.Draw(&projection);
        }
    }
}

InstancedBuffer* Renderer::CreateBuffer(Mesh mesh)
{
    buffers.push_back(InstancedBuffer(mesh));
    u32 index = buffers.size() - 1;
    mesh_by_buffor_index.insert(std::pair(mesh.id, index));
    return &buffers[buffers.size() - 1];
}

InstancedBuffer* Renderer::GetBuffer(std::string mesh_name)
{
    if (mesh_by_buffor_index.contains(mesh_name)) {
        u32 index = mesh_by_buffor_index[mesh_name];
        return &buffers[index];
    }
    return nullptr;
}

InstancedBuffer* Renderer::GetBufferByIndex(u32 index)
{
    return &buffers[index];
}

Renderer R;
