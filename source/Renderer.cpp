#include "Renderer.h"
#include "Buffers.h"
#include "Model.h"

#include <glad/glad.h>

void Renderer::Init() {
	UpdateProjection();

	BufferLayout textured_quad_layout {
		{ BufferElementType::VFloat4, "model"},
		{ BufferElementType::VFloat4, "model1"},
		{ BufferElementType::VFloat4, "model2"},
		{ BufferElementType::VFloat4, "model3"},
		{ BufferElementType::VFloat2, "textureCoords"},
		{ BufferElementType::VFloat2, "textureCoords1"},
		{ BufferElementType::VFloat2, "textureCoords2"},
		{ BufferElementType::VFloat2, "textureCoords3"},
		{ BufferElementType::VFloat4, "color"},
	};

	BufferLayout gradient_layout {
		{ BufferElementType::VFloat4, "model"},
		{ BufferElementType::VFloat4, "model1"},
		{ BufferElementType::VFloat4, "model2"},
		{ BufferElementType::VFloat4, "model3"},
		{ BufferElementType::VFloat4, "colorA"},
		{ BufferElementType::VFloat4, "colorB"},
		{ BufferElementType::VFloat4, "colorC"},
	};

	gradient_buffer.Allocate(MAX_CAPACITY, gradient_layout);
	font_buffer.Allocate(MAX_CAPACITY, textured_quad_layout);
	ui_buffer.Allocate(MAX_CAPACITY, textured_quad_layout);

	ui_buffer.texture_key = "icons";
}

void Renderer::UpdateProjection() {
	projection       = glm::perspective(glm::radians(STATE.window.camera.zoom), (f32)STATE.window.screen_size.x / (f32)STATE.window.screen_size.y, 0.1f, 1000.0f);
	ortho_projection = glm::ortho(0.0f, (f32)STATE.window.screen_size.x, 0.0f, (f32)STATE.window.screen_size.y);
}

void Renderer::Update()
{
}

void Renderer::Draw()
{
    // TODO: temp
    auto shader          = RM.GetShader("object");
    auto texture_shader  = RM.GetShader("texture");
	auto gradient_shader = RM.GetShader("gradient");

	gradient_buffer.Draw(gradient_shader, &ortho_projection);
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

void Renderer::Flush() {
	gradient_buffer.Flush();
	ui_buffer.Flush();
	font_buffer.Flush();
}

void Renderer::Reset() {
	gradient_buffer.Reset();
	ui_buffer.Reset();
	font_buffer.Reset();
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
