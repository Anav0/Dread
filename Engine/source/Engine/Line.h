#pragma once

#include <vector>
#include "Constants.h"


class Shader;

class Line {
    u32 VAO;
    u32 VBO;
    std::vector<v3> points;
    m4 model;

public:
    Line(v3 from, v3 to);
    void Setup();
    void Draw(Shader* shader, m4 projection, m4 view);
    void Redo(v3 from, v3 to);
};
