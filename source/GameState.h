#pragma once

#ifndef GameState_H
#define GameState_H

#include "Camera.h"
#include "WindowManager.h"
#include <set>

namespace RenderMode {
enum RenderMode {
    WIREFRAME,
    NORMAL,
};
}

namespace BoundingDrawMode {
enum BoundingDrawMode {
    SET,
    ALL
};
}

struct GameState {
    WindowManager window;
    RenderMode::RenderMode mode = RenderMode::NORMAL;

    std::set<u32> bounding_boxes_to_draw = {};
    BoundingDrawMode::BoundingDrawMode bounding_draw_mode = BoundingDrawMode::SET;

    bool show_collisions = true;
};

extern GameState STATE;

#endif
