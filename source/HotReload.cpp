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
	CopyFileA("GameCode.dll", "GameCode_tmp.dll", false);
    HINSTANCE dll = LoadLibraryA("GameCode_tmp.dll");
    assert(dll != NULL);

    tGameUpdateAndRender GameUpdateAndRender = (tGameUpdateAndRender) GetProcAddress(dll, "GameUpdateAndRender");
    assert(GameUpdateAndRender != NULL);

    Gradient* card_gradient = (Gradient*) GetProcAddress(dll, "card_gradient");
    assert(card_gradient != NULL);

	return GameCode {
		dll,
		GameUpdateAndRender,
		card_gradient,
	};
}

void UnloadGameCode(GameCode game) {
	FreeLibrary(game.dll);
}
