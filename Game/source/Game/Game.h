#pragma once

#include <array>

#include "Engine/Constants.h";

#define EXPORT extern "C" __declspec(dllexport)

class GameState;

EXPORT void GameUpdateAndRender(WindowManager*);
EXPORT GameState* GameInitEx(GameState, WindowManager*);
EXPORT GameState* GameInit(WindowManager*);
EXPORT void GameInitAfterReload(WindowManager*);
