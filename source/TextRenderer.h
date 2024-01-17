#ifndef TextRenderer_H
#define TextRenderer_H

#include "shader.h"

#include <map>
#include <vector>

#include "stb_truetype.h"

struct Character {
    u32 TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    u32 Advance;
};

class TextRenderer {
public:
    std::map<u32, std::map<char, Character>> CharactersByFontSize;

    Shader TextShader;

    TextRenderer(Shader shader, u32 width, u32 height);

    void Load(std::string font, std::vector<int> fontSizes);

    void ChangeSize(int font_size);

    void RenderText(std::string text, f32 x, f32 y, v3 scale = v3(1), v4 color = v4(1.0f), int font_size = 30);

    glm::ivec2 GetTextSize(std::string text, int font_size = 30);

private:
    u32 VAO, VBO;
};

struct GlyphInfo {
    f32 x, y, w, h;
    u16 advance;
    char character;
};

struct FontInfo {
    std::string font_name;
    u8 font_size;
    std::string path;
    std::map<char, GlyphInfo> glyphs;
    f32 scale;
};

enum class BakeMode {
    AlwaysWrite,
    WriteIfNoneExist,
};

struct TextInBuffer {
    u32 buffer_index = 0;
    u32 pos_in_buffer;

    void ChangeColor(v4 color);
};

class SeanTextRenderer {
    std::vector<FontInfo> fonts;
    std::string font_in_use;

public:
    void BakeFont(std::string font, std::string output_file_name, std::vector<u8> sizes, BakeMode mode = BakeMode::WriteIfNoneExist);
    void UseFont(std::string font_name);
    FontInfo GetCurrentFont(u8 font_size);
};

extern SeanTextRenderer TR;

#endif