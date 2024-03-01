#pragma once

#include "Constants.h"

#include <array>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <windows.h>
#include <winnt.rh>

using namespace std::filesystem;

class WindowManager;

typedef void (*tGameUpdateAndRender)(WindowManager* window);
typedef void (*tGameInit)(WindowManager* window);

// TODO: move to GameState.h
struct GameCode {
    HINSTANCE dll;
    tGameUpdateAndRender GameUpdateAndRender;
    tGameInit GameInit;
    file_time_type dll_change_time;
};

GameCode LoadGameCode();
void UnloadGameCode(GameCode);
void HotReloadGameCode(GameCode* game);
