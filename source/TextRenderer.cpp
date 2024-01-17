#include <iostream>

#include <ft2build.h>
#include <glm/gtc/matrix_transform.hpp>
#include FT_FREETYPE_H

#include "ResourceManager.h"
#include "TextRenderer.h"

#include <freetype/ftsizes.h>
#include <stb_image_write.h>
#include <stb_truetype.h>

#include <vector>

SeanTextRenderer TR;

TextRenderer::TextRenderer(Shader shader, u32 width, u32 height)
{
    this->TextShader = shader;
    this->TextShader.Use();
    this->TextShader.setMat4("projection",
        glm::ortho(0.0f, static_cast<f32>(width), static_cast<f32>(height), 0.0f));
    this->TextShader.setInt("text", 0);

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::Load(std::string font, std::vector<int> font_sizes)
{
    FT_Library ft;

    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    FT_Face face;

    if (FT_New_Face(ft, ("D:\\Projects\\Dread\\fonts\\" + font).c_str(), 0, &face)) // TODO: to relative path
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    std::map<char, Character> characters;
    for (auto font_size : font_sizes) {

        characters.clear();

        FT_Set_Pixel_Sizes(face, 0, font_size);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (u8 c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                abort();
            }

            u32 texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            };
            character.Advance = face->glyph->advance.x;

            characters.insert(std::pair<char, Character>(c, character));
        }

        CharactersByFontSize.insert(std::pair(font_size, std::map(characters)));

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::ChangeSize(int font_size)
{
}

void TextRenderer::RenderText(std::string text, f32 x, f32 y, v3 scale, v4 color, int font_size)
{
    this->TextShader.Use();
    this->TextShader.setVec4("textColor", color);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    std::string::const_iterator c;

    auto Characters = CharactersByFontSize[font_size];

    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        f32 xpos = x + ch.Bearing.x * scale.x;
        f32 ypos = y + (Characters['H'].Bearing.y - ch.Bearing.y) * scale.y;

        f32 w = ch.Size.x * scale.x;
        f32 h = ch.Size.y * scale.y;

        f32 vertices[6][4] = {
            { xpos, ypos + h, 0.0f, 1.0f },
            { xpos + w, ypos, 1.0f, 0.0f },
            { xpos, ypos, 0.0f, 0.0f },

            { xpos, ypos + h, 0.0f, 1.0f },
            { xpos + w, ypos + h, 1.0f, 1.0f },
            { xpos + w, ypos, 1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale.x;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

glm::ivec2 TextRenderer::GetTextSize(std::string text, int font_size)
{
    int width = 0;
    int height = 0;

    std::string::const_iterator c;

    auto& Characters = CharactersByFontSize[font_size]; //@Robust

    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];
        width += ch.Size.x;
        if (ch.Size.y > height)
            height = ch.Size.y;
    }

    assert(width > 0);
    assert(height > 0);

    return glm::ivec2(width, height);
}

void SeanTextRenderer::BakeFont(std::string font, std::string output_file_name, std::vector<u8> sizes, BakeMode mode)
{
    long size;
    unsigned char* fontBuffer;

    // TODO: to relative path
    FILE* fontFile = fopen((FONTS_PATH + '/' + font).c_str(), "rb");
    fseek(fontFile, 0, SEEK_END);
    size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    fontBuffer = (unsigned char*)malloc(size);

    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    constexpr int CHANNEL = 2;

    for (auto font_size : sizes) {

        FontInfo font_info;
        stbtt_fontinfo info;

        if (!stbtt_InitFont(&info, fontBuffer, 0)) {
            printf("failed\n");
        }

        int b_w = 0; // Padding
        int b_h = font_size;
        int l_h = font_size;
        const char from = 32;
        const char to = 127;
        //Onlu uppercase
        //const char from = 65;
        //const char to = 90;

        float scale = stbtt_ScaleForPixelHeight(&info, l_h);
        font_info.scale = scale;
        for (char i = from; i < to; i++) {
            int c_x1, c_y1, c_x2, c_y2;
            stbtt_GetCodepointBitmapBox(&info, (char)i, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
            b_w += c_x2 - c_x1 + 20;
        }

        int baseline, ascent, descent, lineGap;
        stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

        baseline = roundf(ascent * scale);
        descent = roundf(descent * scale);

        unsigned char* bitmap = (unsigned char*)calloc(b_w * b_h, sizeof(unsigned char));

        int x = 0;

        auto file_name = FONTS_PATH + "/baked/" + output_file_name + "_" + std::to_string(font_size) + ".png";

        font_info.font_size = l_h;
        font_info.font_name = font;
        font_info.path = file_name;

        for (char i = from; i < to; ++i) {
            GlyphInfo glyph;

            int ax;
            int lsb;
            stbtt_GetCodepointHMetrics(&info, (char)i, &ax, &lsb);

            int c_x1, c_y1, c_x2, c_y2;
            stbtt_GetCodepointBitmapBox(&info, (char)i, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

            int y = baseline + c_y1;

            int byteOffset = x + roundf(lsb * scale) + (y * b_w);
            stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, (char)i);

            glyph.character = i;
            glyph.advance = ax;
            glyph.h = c_y2 - c_y1;
            glyph.w = c_x2 - c_x1;
            glyph.x = x + roundf(lsb * scale);
            glyph.y = y;

            font_info.glyphs.insert(std::pair(i, glyph));

            x += roundf(ax * scale);
        }

        u8* png_data = new u8[b_w * b_h * CHANNEL];
        int i = 0;
        int j = 0;
        for (size_t x = 0; x < b_w; x++) {
            for (size_t y = 0; y < b_h; y++) {
                u8 alpha = 255;
                if (bitmap[j] == '\0')
                    alpha = 0;

                png_data[i++] = bitmap[j++]; // R
                png_data[i++] = alpha; // A
            }
        }

        fonts.push_back(font_info);

        bool file_exists = fopen(file_name.c_str(), "r");
        if (mode == BakeMode::AlwaysWrite || (mode == BakeMode::WriteIfNoneExist && !file_exists)) {
            stbi_write_png(file_name.c_str(), b_w, b_h, CHANNEL, png_data, b_w * CHANNEL); // b_w * 4 gives us transparency
        }

        free(bitmap);
    }

    free(fontBuffer);
}

void SeanTextRenderer::UseFont(std::string font_name)
{
    this->font_in_use = font_name;
}

FontInfo SeanTextRenderer::GetCurrentFont(u8 font_size)
{
    for (auto& font : fonts) {
        if (font.font_name == this->font_in_use && font.font_size == font_size) {
            return font;
        }
    }

    assert(false); // TODO: error handling
    return fonts[0];
}
