#pragma once

#include <vector>

#include "Misc.h"
#include "Shader.h"

#define MAX_BONE_INFLUENCE 4

enum class AlignX {
    Left,
    Middle,
    Right,
};

enum class AlignY {
    Top,
    Center,
    Bottom,
};

struct Vertex {
    v3 Position;
    v3 Normal;
    v2 TexCoords = { -1, -1 };

    Vertex() { }
    Vertex(v3 pos) { Position = pos; };
};

struct Transform {
    v3 position, size, scale;

    f32 rotation;
};

enum class EntityType {
    BoundingBox,
    Oblast,
};

class Entity {
public:
    Transform transform;
    virtual bool Update() = 0;
};
