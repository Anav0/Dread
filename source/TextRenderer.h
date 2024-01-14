#ifndef TextRenderer_H
#define TextRenderer_H

#include <map>

#include "shader.h"
#include <vector>

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

#endif