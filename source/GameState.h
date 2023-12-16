#pragma once

#ifndef GameState_H
#define GameState_H

#include "ResourceManager.h"
#include "WindowManager.h"

struct GameState {
    WindowManager window;
    ResourceManager resources;


 };

extern GameState STATE;

#endif
