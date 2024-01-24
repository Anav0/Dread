#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "ResourceManager.h"
#include "TextRenderer.h"

#include <stb_image_write.h>
#include <stb_truetype.h>

#include <vector>
#include "Renderer.h"

SeanTextRenderer TR;

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
            glyph.offset_y = c_y1;

            int byteOffset = x + roundf(lsb * scale) + (y * b_w);
            stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, (char)i);

            glyph.character = i;
            glyph.advance = round(ax * scale);
            if (i == ' ') {
                glyph.h = l_h * 0.25;
                glyph.w = l_h * 0.25;
                glyph.x = -999;
                glyph.y = -999;
            } else {
                glyph.h = c_y2 - c_y1;
                glyph.w = c_x2 - c_x1;
                glyph.x = x + roundf(lsb * scale);
                glyph.y = descent * -1;
            }

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

v2 SeanTextRenderer::GetTextSize(char* text, u8 font_size)
{
    return v2();
}

void TextInBuffer::ChangeColor(v4 color)
{
    for (size_t i = pos_in_buffer; i < pos_in_buffer+length; i++) {
        R.ui_buffer.UpdateColor(i, color);
    }
}

void TextInBuffer::ChangeText(char* text)
{
    for (size_t i = pos_in_buffer; i < pos_in_buffer+length; i++) {
    }
}
