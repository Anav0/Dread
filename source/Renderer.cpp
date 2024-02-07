#include "Renderer.h"
#include "Buffers.h"
#include "Model.h"

#include <glad/glad.h>

void Renderer::Init() {
	projection       = glm::perspective(glm::radians(STATE.window.camera.zoom), (f32)STATE.window.screen_size.x / (f32)STATE.window.screen_size.y, 0.1f, 1000.0f);
	ortho_projection = glm::ortho(0.0f, (f32)STATE.window.screen_size.x, 0.0f, (f32)STATE.window.screen_size.y);

	font_buffer.Allocate();
	ui_buffer.Allocate();
	ui_buffer.texture_key = "icons";
}

void Renderer::Update()
{
}

void Renderer::Draw()
{
    // TODO: temp
    auto shader         = RM.GetShader("object");
    auto texture_shader = RM.GetShader("texture");

	ui_buffer.Draw(texture_shader, &ortho_projection);
	font_buffer.Draw(texture_shader, &ortho_projection);

    for (auto& buffer : buffers) {
        buffer.Draw(shader, &projection, nullptr);
    }

    if (STATE.show_collisions) {
        for (auto& box : R.boxes) {
            box.Draw(&projection);
        }
    }
}

InstancedMeshBuffer* Renderer::CreateBuffer(Mesh mesh)
{
    buffers.push_back(InstancedMeshBuffer(mesh));
    u32 index = buffers.size() - 1;
    mesh_by_buffor_index.insert(std::pair(mesh.id, index));
    return &buffers[buffers.size() - 1];
}

InstancedMeshBuffer* Renderer::GetBuffer(std::string mesh_name)
{
    if (mesh_by_buffor_index.contains(mesh_name)) {
        u32 index = mesh_by_buffor_index[mesh_name];
        return &buffers[index];
    }
    return nullptr;
}

InstancedMeshBuffer* Renderer::GetBufferByIndex(u32 index)
{
    return &buffers[index];
}

Renderer R;
