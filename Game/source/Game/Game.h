#pragma once

#include <array>

#include "Engine/Constants.h";

#define EXPORT extern "C" __declspec(dllexport)

EXPORT void GameUpdateAndRender(WindowManager*);
EXPORT void GameInit(WindowManager*);
