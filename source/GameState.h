#pragma once

#ifndef GameState_H
#define GameState_H

#include "Camera.h"
#include "WindowManager.h"
#include <set>

enum class RenderMode {
    WIREFRAME,
    NORMAL,
};

enum class BoundingDrawMode {
    SET,
    ALL
};

struct GameState {
    WindowManager window;
    RenderMode mode = RenderMode::NORMAL;

    std::set<u32> bounding_boxes_to_draw = {};
    BoundingDrawMode bounding_draw_mode = BoundingDrawMode::SET;

    bool show_collisions = true;
};

extern GameState STATE;

#endif
