#pragma once

#ifndef GameState_H
#define GameState_H

#include "Camera.h"
#include "Constants.h"
#include "Entities.h"
#include "WindowManager.h"

#include <array>
#include <set>
#include <map>

enum class RenderMode {
    WIREFRAME,
    NORMAL,
};

constexpr static u8 NUMBER_OF_OBLASTS = 25;
constexpr static u8 NUMBER_OF_SUPPORTERS = 10;

struct GameState {
    // Other
    WindowManager window;
    RenderMode mode = RenderMode::NORMAL;
    bool show_collisions = false;

    Oblast* selected_oblast = nullptr;
};

Oblast* GetOblast(OblastCode code);

v2 GetScreenSize();
void ChangeControl(f32 by);

extern GameState STATE;

#endif
