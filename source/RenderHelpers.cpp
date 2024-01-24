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

#include "stb_truetype.h"

std::vector<MeshInBuffer> AddModel(v3 position, std::string model_name, v4 color, f32 rotation, f32 scale)
{
    Model* model = RM.GetModel(model_name);

    std::vector<MeshInBuffer> meshes;

    for (auto& mesh : model->meshes) {
        InstancedMeshBuffer* buffer;
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

TextInBuffer AddText(std::string text, v2 pos, v4 color, u8 font_size)
{
    FontInfo font = TR.GetCurrentFont(font_size);

    Texture* font_atlas = RM.GetTexture(font.path);
    if (font_atlas == nullptr)
        font_atlas = RM.LoadTexture(font.path, font.path, true, true);

    R.ui_buffer.texture_key = font.path;

    f32 base_y = pos.y;
    TextInBuffer handle {}; 
    handle.pos_in_buffer = R.ui_buffer.GetCurrentIndex();
    for (size_t i = 0; i < text.size(); i++) {
        char c = text[i];

        GlyphInfo glyph = font.glyphs[c];

        AtlasTextureInfo texture_info;
        texture_info.position = v2(glyph.x, glyph.y);
        texture_info.scale = v2(1.0f, 1.0f);
        texture_info.size = v2(glyph.w, glyph.h);

        R.ui_buffer.AddTexturedRect(&texture_info, font_atlas, pos, texture_info.size, 0, color);
        handle.length += 1;

        //pos.x += glyph.advance;
        pos.x += glyph.w + 1;
        pos.y = base_y;
    }

    return handle;
}

TextInBuffer AddText(u8 size, v2 pos, v4 color, u8 font_size)
{
    FontInfo font = TR.GetCurrentFont(font_size);

    Texture* font_atlas = RM.GetTexture(font.path);
    if (font_atlas == nullptr)
        font_atlas = RM.LoadTexture(font.path, font.path, true, true);

    R.ui_buffer.texture_key = font.path;

    f32 base_y = pos.y;
    TextInBuffer handle {}; 
    handle.pos_in_buffer = R.ui_buffer.GetCurrentIndex();
    for (size_t i = 0; i < size; i++) {
        char c = ' ';

        GlyphInfo glyph = font.glyphs[c];

        AtlasTextureInfo texture_info;
        texture_info.position = v2(glyph.x, glyph.y);
        texture_info.scale = v2(1.0f, 1.0f);
        texture_info.size = v2(glyph.w, glyph.h);

        R.ui_buffer.AddTexturedRect(&texture_info, font_atlas, pos, texture_info.size, 0, color);
        handle.length += 1;

        //pos.x += glyph.advance;
        pos.x += glyph.w + 1;
        pos.y = base_y;
    }

    return handle;
}

std::vector<MeshInBuffer> AddModel(v3 position, v3 size, std::string model_name, v4 color = { 0.0f, 0.0f, 0.0f, 1.0f }, f32 rotation = 0.0f, f32 scale = 1.0f)
{
    Model* model = RM.GetModel(model_name);

    std::vector<MeshInBuffer> meshes;

    for (auto& mesh : model->meshes) {
        InstancedMeshBuffer* buffer;
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
