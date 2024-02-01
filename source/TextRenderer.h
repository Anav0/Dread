#ifndef TextRenderer_H
#define TextRenderer_H

#include "shader.h"
#include "Constants.h"

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

struct TextInBuffer {
	v2 last_pos;
	u8 last_size;
    u32 pos_in_buffer = 0;
    u32 length = 0;

    void ChangeColor(v4 color);
    void ChangeText(std::string text, u8 size, v2 pos);
	void ChangeText(std::string text);
};

class SeanTextRenderer {
    std::vector<FontInfo> fonts;
    std::string font_in_use;

public:
    void BakeFont(std::string font, std::string output_file_name, std::vector<u8> sizes, BakeMode mode = BakeMode::WriteIfNoneExist);
    void UseFont(std::string font_name);
    FontInfo GetCurrentFont(u8 font_size);
    v2 GetTextSize(const char* text, u8 font_size);
};

extern SeanTextRenderer TR;

#endif