#include "Renderer.h"
#include "Buffers.h"
#include "Model.h"
#include "Particles.h"

#include <glad/glad.h>

void Renderer::Init(Camera& camera, v2 screen_size)
{
    UpdateProjection(camera, screen_size);

    BufferLayout gradient_layout {
        { BufferElementType::VFloat4, "pos_and_size" },
        { BufferElementType::VFloat4, "model_0" },
        { BufferElementType::VFloat4, "model_1" },
        { BufferElementType::VFloat4, "model_2" },
        { BufferElementType::VFloat4, "model_3" },
        { BufferElementType::Int, "gradient_type" },
        { BufferElementType::VFloat2, "middle" },
        { BufferElementType::Float, "radial_factor" },
        { BufferElementType::VFloat4, "radial_position_and_smoothing" },
        { BufferElementType::VFloat4, "color_0" },
        { BufferElementType::VFloat4, "color_1" },
        { BufferElementType::VFloat4, "color_2" },
        { BufferElementType::VFloat4, "color_3" },
    };

    gradient_buffer.Allocate(MAX_CAPACITY, gradient_layout);

    // UI buffer
    CreateQuadBuffer(MAX_CAPACITY, "texture", "icons");
	// Insignia buffer
    CreateQuadBuffer(MAX_CAPACITY, "texture", "insignia");
    // Font buffer
    CreateQuadBuffer(MAX_CAPACITY, "texture", "");
}

void Renderer::UpdateProjection(Camera& camera, v2 screen_size)
{
    projection = glm::perspective(glm::radians(camera.zoom), (f32)screen_size.x / (f32)screen_size.y, 0.1f, 1000.0f);
    ortho_projection = glm::ortho(0.0f, (f32)screen_size.x, 0.0f, (f32)screen_size.y);
}

void Renderer::Update(f32 dt)
{
    for (auto& e : emitters) {
        e.Update(dt);
    }
}

void Renderer::DrawModels(Shader* shader, Camera& camera, v2 screen_size)
{
    auto view = camera.GetViewMatrix();
    for (auto& buffer : buffers) {
        buffer.Draw(shader, screen_size, projection, view, nullptr);
    }
}

// NOTE: This is not flexible enough. I want to be able to change shader 
// by just calling set_shader(shader);
void Renderer::Draw(Camera& camera, v2 screen_size)
{
    auto gradient_shader = RM.GetShader("gradient");
    auto particles_shader = RM.GetShader("particles");

    auto view = camera.GetViewMatrix();

    gradient_buffer.Draw(screen_size, gradient_shader, ortho_projection);

    for (ParticlesEmitter& e : emitters) {
        e.Draw(particles_shader, ortho_projection);
    }

    for (auto& b : quad_buffers) {
        b.Draw(ortho_projection);
    }
}

void Renderer::FlushModels()
{
	for(auto& b : buffers) {
		b.Flush();
	}
}

void Renderer::Flush()
{
    gradient_buffer.Flush();
    for (ParticlesEmitter& e : emitters) {
        e.Flush();
    }

    for (auto& b : quad_buffers) {
        b.Flush();
    }
}

void Renderer::Reset()
{
    gradient_buffer.Reset();

    for (auto& b : quad_buffers) {
        b.Reset();
    }
}

void Renderer::ScaleAllModels(f32 scale)
{
    for (auto& b : buffers) {
        b.ScaleAll(scale);
    }
}

InstancedMeshBuffer* Renderer::CreateBuffer(u32 size, Mesh mesh)
{
    static BufferLayout mesh_layout {
        { BufferElementType::VFloat4, "aModel" },
        { BufferElementType::VFloat4, "aModel1" },
        { BufferElementType::VFloat4, "aModel2" },
        { BufferElementType::VFloat4, "aModel3" },
        { BufferElementType::VFloat4, "color" },
        { BufferElementType::Int, "aEntityId" },
    };

    buffers.push_back(InstancedMeshBuffer(mesh));
    u32 index = buffers.size() - 1;
    mesh_by_buffor_index.insert(std::pair(mesh.id, index));
		auto b = &buffers[buffers.size() - 1];
		b->Allocate(size, mesh_layout);
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

u32 Renderer::CreateQuadBuffer(u32 size, std::string shader_key, std::string texture_key)
{
		static BufferLayout layout {
        { BufferElementType::VFloat2, "pos" },
        { BufferElementType::VFloat2, "size" },
        { BufferElementType::VFloat4, "model" },
        { BufferElementType::VFloat4, "model1" },
        { BufferElementType::VFloat4, "model2" },
        { BufferElementType::VFloat4, "model3" },
        { BufferElementType::VFloat2, "textureCoords" },
        { BufferElementType::VFloat2, "textureCoords1" },
        { BufferElementType::VFloat2, "textureCoords2" },
        { BufferElementType::VFloat2, "textureCoords3" },
        { BufferElementType::VFloat4, "color" },
    };
    TexturedQuadBuffer buffer;
    buffer.texture_key = texture_key;
    buffer.shader_key = shader_key;
	buffer.Allocate(size, layout);
    quad_buffers.push_back(buffer);
    return quad_buffers.size() - 1;
}

TexturedQuadBuffer& Renderer::GetQuadBuffer(u32 index)
{
    return quad_buffers[index];
}

Renderer R;
