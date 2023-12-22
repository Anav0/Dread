#pragma once

#ifndef GameState_H
#define GameState_H

#include "Camera.h"
#include "WindowManager.h"

namespace RenderMode {
enum RenderMode {
    WIREFRAME,
    NORMAL,
};
}

struct GameState {
    WindowManager window;
    RenderMode::RenderMode mode = RenderMode::NORMAL;

    bool show_collisions = true;
};

extern GameState STATE;

#endif
