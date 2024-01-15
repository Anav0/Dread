#include "RenderHelpers.h"
#include "Base.h"
#include "BoundingBox.h"
#include "Collision.h"
#include "EntityManager.h"
#include "Line.h"
#include "Model.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TextRenderer.h"

std::vector<MeshInBuffer> AddModel(v3 position, std::string model_name, v4 color, f32 rotation, f32 scale)
{
    Model* model = RM.GetModel(model_name);

    std::vector<MeshInBuffer> meshes;

    for (auto& mesh : model->meshes) {
        InstancedBuffer* buffer;
        buffer = R.GetBuffer(mesh.id);
        if (buffer == nullptr)
            buffer = R.CreateBuffer(mesh);

        auto mesh_in_buffer = buffer->AddMesh(position, color, rotation, scale);
        mesh_in_buffer.buffer_index = R.buffers.size() - 1;
        meshes.push_back(mesh_in_buffer);
    }

    assert(meshes.size() > 0);

    return meshes;
}

void AddText(std::string text, v2 pos, v4 color)
{
    auto font_size = 48;
    FontInfo font = TR.GetCurrentFont(font_size);

    Texture* font_atlas = RM.GetTexture(font.path);
    if (font_atlas == nullptr)
        font_atlas = RM.LoadTexture(font.path, font.path, true, true);

    R.ui_buffer.texture_key = font.path;

    for (char c : text) {
        GlyphInfo glyph = font.glyphs[c];

        AtlasTextureInfo texture_info;
        texture_info.position = v2(glyph.x, glyph.y);
        texture_info.scale = v2(1.0f, 1.0f);
        texture_info.size = v2(glyph.w, glyph.h);

        R.ui_buffer.AddTexturedRect(&texture_info, font_atlas, pos, texture_info.size, 0, color);
        pos.x += glyph.w;
    }
}

std::vector<MeshInBuffer> AddModel(v3 position, v3 size, std::string model_name, v4 color = { 0.0f, 0.0f, 0.0f, 1.0f }, f32 rotation = 0.0f, f32 scale = 1.0f)
{
    Model* model = RM.GetModel(model_name);

    std::vector<MeshInBuffer> meshes;

    for (auto& mesh : model->meshes) {
        InstancedBuffer* buffer;
        buffer = R.GetBuffer(mesh.id);
        if (buffer == nullptr)
            buffer = R.CreateBuffer(mesh);

        auto mesh_in_buffer = buffer->AddMesh(position, size, color, rotation, scale);
        mesh_in_buffer.buffer_index = R.buffers.size() - 1;
        meshes.push_back(mesh_in_buffer);
    }

    assert(meshes.size() > 0);

    return meshes;
}
