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
    HINSTANCE dll = LoadLibrary(TEXT("GameCode.dll"));
    assert(dll != NULL);

    tGameUpdateAndRender GameUpdateAndRender = (tGameUpdateAndRender) GetProcAddress(dll, "GameUpdateAndRender");
    assert(GameUpdateAndRender != NULL);

	return GameCode {
		dll,
		GameUpdateAndRender,
	};
}

void UnloadGameCode(GameCode game) {
	FreeLibrary(game.dll);
}
