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
class GameState;

typedef void (*tGameUpdateAndRender)(WindowManager* window);
typedef void (*tGameInitAfterReload)(WindowManager* window);

typedef GameState* (*tGameInit)(WindowManager* window);
typedef GameState* (*tGameInitEx)(GameState state, WindowManager* window);

// TODO: move to GameState.h
struct GameCode {
    HINSTANCE dll;
    tGameUpdateAndRender GameUpdateAndRender;
    tGameInit GameInit;
    tGameInitEx GameInitEx;
    tGameInitAfterReload GameInitAfterReload;
    file_time_type dll_change_time;
};

bool GameCodeChanged(GameCode*);
GameCode LoadGameCode();
void UnloadGameCode(GameCode*);
