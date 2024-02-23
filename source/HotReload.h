#pragma once


#include "Constants.h" 

#include <string>
#include <array>
#include <windows.h> 
#include <stdio.h> 
#include <cassert>
#include <winnt.rh>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std::filesystem;

typedef void (*tGameUpdateAndRender) (void);

//TODO: move to GameState.h
struct GameCode {
	HINSTANCE dll;
	tGameUpdateAndRender GameUpdateAndRender;
	Gradient* card_gradient;
	file_time_type dll_change_time;
};

GameCode LoadGameCode();
void UnloadGameCode(GameCode);
void HotReloadGameCode(GameCode* game);
