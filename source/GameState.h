#pragma once

#ifndef GameState_H
#define GameState_H

#include "Camera.h"
#include "ResourceManager.h"
#include "WindowManager.h"

namespace RenderMode {
enum RenderMode {
    WIREFRAME,
    NORMAL,
};
}

struct GameState {
    WindowManager window;
    ResourceManager resources;
    RenderMode::RenderMode mode = RenderMode::NORMAL;

    bool show_collisions = true;
};

extern GameState STATE;

#endif
