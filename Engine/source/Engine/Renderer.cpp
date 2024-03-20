#include "Renderer.h"
#include "Buffers.h"
#include "Buffers.h"
#include "Model.h"
#include "Particles.h"

#include <glad/glad.h>


void Renderer::Init(Camera& camera, v2 screen_size) {
	UpdateProjection(camera, screen_size);

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
		{ BufferElementType::VFloat4, "model_0"},
		{ BufferElementType::VFloat4, "model_1"},
		{ BufferElementType::VFloat4, "model_2"},
		{ BufferElementType::VFloat4, "model_3"},
		{ BufferElementType::Int,     "gradient_type"},
		{ BufferElementType::VFloat2, "middle"},
		{ BufferElementType::Float,   "radial_factor"},
		{ BufferElementType::VFloat2, "radial_position"},
		{ BufferElementType::VFloat4, "color_0"},
		{ BufferElementType::VFloat4, "color_1"},
		{ BufferElementType::VFloat4, "color_2"},
		{ BufferElementType::VFloat4, "color_3"},
	};


	gradient_buffer.Allocate(MAX_CAPACITY, gradient_layout);
	font_buffer.Allocate(MAX_CAPACITY, textured_quad_layout);
	ui_buffer.Allocate(MAX_CAPACITY, textured_quad_layout);

	ui_buffer.texture_key = "icons";
}

void Renderer::UpdateProjection(Camera& camera, v2 screen_size) {
	projection       = glm::perspective(glm::radians(camera.zoom), (f32)screen_size.x / (f32)screen_size.y, 0.1f, 1000.0f);
	ortho_projection = glm::ortho(0.0f, (f32)screen_size.x, 0.0f, (f32)screen_size.y);
}

void Renderer::Update(f32 dt)
{
	for(auto& e : emitters) {
		e.Update(dt);
	}
}

void Renderer::DrawModels(Shader* pshader, PickingBuffer* picking, Camera& camera, v2 screen_size) {
		auto view = camera.GetViewMatrix();
    for (auto& buffer : buffers) {
        buffer.Draw(pshader, screen_size, picking, projection, view, nullptr);
    }
}

void Renderer::Draw(Shader* pshader, PickingBuffer* picking, Camera& camera, v2 screen_size)
{
  auto texture_shader   = RM.GetShader("texture");
	auto gradient_shader  = RM.GetShader("gradient");
	auto particles_shader = RM.GetShader("particles");

	auto view = camera.GetViewMatrix();

	gradient_buffer.Draw(screen_size, gradient_shader, ortho_projection);
	for(ParticlesEmitter& e : emitters) {
		e.Draw(particles_shader , ortho_projection);
	}
	ui_buffer.Draw(texture_shader, ortho_projection);
	font_buffer.Draw(texture_shader, ortho_projection);
}

void Renderer::Flush() {
	gradient_buffer.Flush();
	for(ParticlesEmitter& e : emitters) {
		e.Flush();
	}
	ui_buffer.Flush();
	font_buffer.Flush();
}

void Renderer::Reset() {
	gradient_buffer.Reset();
	ui_buffer.Reset();
	font_buffer.Reset();
}

void Renderer::ScaleAllModels(f32 scale) {
	for(auto& b : buffers) {
		b.ScaleAll(scale);
	}
}

InstancedMeshBuffer* Renderer::CreateBuffer(Mesh mesh)
{
    buffers.push_back(InstancedMeshBuffer(mesh));
    u32 index = buffers.size() - 1;
    mesh_by_buffor_index.insert(std::pair(mesh.id, index));
    return &buffers[buffers.size() - 1];
}

InstancedMeshBuffer* Renderer::GetBuffer(const std::string& mesh_name)
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
