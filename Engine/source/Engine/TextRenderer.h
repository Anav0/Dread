#ifndef TextRenderer_H
#define TextRenderer_H

#include "shader.h"
#include "GLM.h"
#include "Misc/Constants.h"

#include <map>
#include <vector>
#include <string>

#include "stb_truetype.h"

struct GlyphInfo {
    f32 x, y, w, h;
    u16 advance;
    i16 offset_y;
    char character;
};

struct FontInfo {
    std::string font_name;
    u8 font_size;
    u8 space_width, space_height;
    std::string path;
    std::map<char, GlyphInfo> glyphs;
    f32 scale;
};

enum class BakeMode {
    AlwaysWrite,
    WriteIfNoneExist,
};

class SeanTextRenderer {
    std::vector<FontInfo> fonts;
    std::string font_in_use;

public:
    void BakeFont(std::string font, std::string output_file_name, std::vector<u8> sizes, BakeMode mode = BakeMode::WriteIfNoneExist);
    void UseFont(std::string font_name);
    FontInfo GetCurrentFont(u8 font_size);
    v2 GetTextSize(const char* label, u8 font_size);
};

extern SeanTextRenderer TR;

#endif