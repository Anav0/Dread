#pragma once

#include <string>

#include <windows.h> 
#include <stdio.h> 
#include <cassert>
#include <winnt.rh>

//TODO: move to GameState.h
struct GameCode;

typedef void (*tGameUpdateAndRender) (void);

struct GameCode {
	HINSTANCE dll;
	tGameUpdateAndRender GameUpdateAndRender;
};


GameCode LoadGameCode();
void UnloadGameCode(GameCode);