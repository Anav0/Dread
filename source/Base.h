#pragma once

#include <vector>

#include "GameState.h"
#include "Misc.h"
#include "Shader.h"

enum AlignX {
    Left,
    Middle,
    Right,
};

enum AlignY {
    Top,
    Center,
    Bottom,
};

struct Transform {
    v3 position, size, scale;

    float rotation;
};

struct UiTransform {
    v2 size, position;
    m4* projection;
    float rotation = 0.0f;
    AlignX align_x;
    AlignY align_y;

    float padding[4] = { 0.0, 0.0, 0.0, 0.0 };
    v3 scale = v3(1.0f);
};

struct UiColor {
    v4 fg_color, fg_hover, bg_color, bg_hover;
};

enum EntityType {
    GameCard,
    Rect
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
