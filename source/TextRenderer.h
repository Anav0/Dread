#ifndef TextRenderer_H
#define TextRenderer_H

#include <map>

#include "shader.h"
#include <vector>

struct Character {
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};

class TextRenderer {
public:
    std::map<unsigned int, std::map<char, Character>> CharactersByFontSize;

    Shader TextShader;

    TextRenderer(Shader shader, unsigned int width, unsigned int height);

    void Load(std::string font, std::vector<int> fontSizes);

    void ChangeSize(int font_size);

    void RenderText(std::string text, float x, float y, v3 scale = v3(1), v4 color = v4(1.0f), int font_size = 30);

    glm::ivec2 GetTextSize(std::string text, int font_size = 30);

private:
    unsigned int VAO, VBO;
};

#endif