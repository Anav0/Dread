#pragma once

#ifndef GameState_H
#define GameState_H

#include "Camera.h"
#include "WindowManager.h"
#include "Constants.h"
#include "Entities.h"

#include <set>
#include <array>

enum class RenderMode {
    WIREFRAME,
    NORMAL,
};

constexpr static u8 NUMBER_OF_OBLASTS    = 26;
constexpr static u8 NUMBER_OF_SUPPORTERS = 10;

struct GameState {
    // Other
    WindowManager window;
    RenderMode mode = RenderMode::NORMAL;
    bool show_collisions = true;

    // Game state
    std::array<Oblast, NUMBER_OF_OBLASTS>    oblasts;
    std::array<Oblast, NUMBER_OF_SUPPORTERS> supporters;
    
};

extern GameState STATE;

#endif
