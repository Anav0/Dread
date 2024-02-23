#pragma once

#include <string>
#include <array>

#include "Constants.h" 

#include <windows.h> 
#include <stdio.h> 
#include <cassert>
#include <winnt.rh>

typedef void (*tGameUpdateAndRender) (void);

//TODO: move to GameState.h
struct GameCode {
	HINSTANCE dll;
	tGameUpdateAndRender GameUpdateAndRender;
	Gradient* card_gradient;
};

GameCode LoadGameCode();
void UnloadGameCode(GameCode);
