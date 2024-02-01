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

constexpr static u8 NUMBER_OF_OBLASTS    = 25;
constexpr static u8 NUMBER_OF_SUPPORTERS = 10;

constexpr bool HIDE_ALPHA  = true;
constexpr bool DEBUG_LINES = false;
constexpr bool CULLING     = true;

constexpr const char* MONTHS[] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

struct GameState {
    // Other
    WindowManager window;
    RenderMode mode = RenderMode::NORMAL;
    bool show_collisions = false;

    Oblast* selected_oblast = nullptr;

    f32 popular_support = 0.8;
    u64 reserve         = 600'000;

	std::vector<Country> countries;

	u8 current_turn = 1;
	u8 max_turn     = 12 * 3; //Three years, 12 months per year 3 years

	bool turn_changed = false;
	
};

const char* GetMonth();
std::string GetDateStr();
std::string GetYear();

Oblast* GetOblast(OblastCode code);

v2 GetScreenSize();
void ChangeControl(f32 by);
void GoToNextPhase();
void GoToNextTurn();
void ComputerPhase();
void DrawEndScreen();

extern GameState STATE;

#endif
