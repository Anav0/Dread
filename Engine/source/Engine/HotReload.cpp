#include "HotReload.h"

// Windows
#include <windows.h> 
#include <stdio.h> 
#include <cassert>
#include <winnt.rh>

static LPWSTR ToWeirdMicrosoftString(std::string text)
{
    wchar_t wtext[20];
    mbstowcs(wtext, text.c_str(), text.length()); // includes null
    return wtext;
}

GameCode LoadGameCode() {
	  bool success = CopyFileA("Game.dll", "Game_tmp.dll", false);
		if(!success) { 
			printf("Failed to copy DLL: '%i'\n", GetLastError());
		}
		if(success)  printf("Success to copy DLL\n");

    HINSTANCE dll = LoadLibraryA("Game_tmp.dll");
    assert(dll != NULL);

    tGameUpdateAndRender GameUpdateAndRender = (tGameUpdateAndRender) GetProcAddress(dll, "GameUpdateAndRender");
    assert(GameUpdateAndRender != NULL);

	tGameInit GameInit = (tGameInit) GetProcAddress(dll, "GameInit");
    assert(GameInit != NULL);
    auto write_time = last_write_time("Game.dll");

	return GameCode {
		dll,
		GameUpdateAndRender,
        GameInit,
		write_time
	};
}

void UnloadGameCode(GameCode* game) {
	FreeLibrary(game->dll);
	game->dll = 0;
}

void HotReloadGameCode(GameCode* game) {

	auto dll_last_written = last_write_time("Game.dll");
	bool dll_was_changed  = dll_last_written > game->dll_change_time;

	if (dll_was_changed) {
		UnloadGameCode(game);
		*game = LoadGameCode();
  } 
}
