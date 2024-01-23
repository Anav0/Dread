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

    // Game state
    std::array<Oblast, NUMBER_OF_OBLASTS> oblasts;
    std::array<Oblast, NUMBER_OF_SUPPORTERS> supporters;
};

Oblast* GetOblast(OblastCode code);

extern GameState STATE;

#endif
