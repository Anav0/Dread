#pragma once

#include <vector>

#include "GameState.h"
#include "Misc.h"
#include "Shader.h"

#define MAX_BONE_INFLUENCE 4

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

struct Vertex {
    v3 Position;
    v3 Normal;
    v2 TexCoords = { -1, -1 };
    v3 Tangent;
    v3 Bitangent;
    v4 Color = GREY;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    f32 m_Weights[MAX_BONE_INFLUENCE];

    Vertex() { }
    Vertex(v3 pos) { Position = pos; };
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
    Rect,
    eBoundingBox,
    eModel,
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
