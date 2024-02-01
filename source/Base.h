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

class Rectangle : public Entity {
public:
    v4 color;

    Rectangle() { }

    Rectangle(Transform transform, v4 color = { 1.0, 1.0, 1.0, 1.0 })
    {
        this->color = color;
        this->transform = transform;
    }

    // Inherited via Entity
    bool Update() override;
};
