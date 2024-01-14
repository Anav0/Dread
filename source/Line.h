#pragma once

#include <vector>
#include "Constants.h"


class Shader;

class Line {
    unsigned int VAO;
    unsigned int VBO;
    std::vector<v3> points;
    m4 model;

public:
    Line(v3 from, v3 to);
    void Setup();
    void Draw(Shader* shader, m4* projection);
    void Redo(v3 from, v3 to);
};
