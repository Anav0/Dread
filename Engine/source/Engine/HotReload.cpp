#include "HotReload.h"

// Windows
#include <cassert>
#include <stdio.h>
#include <windows.h>
#include <winnt.rh>

static LPWSTR ToWeirdMicrosoftString(std::string text)
{
    wchar_t wtext[20];
    mbstowcs(wtext, text.c_str(), text.length()); // includes null
    return wtext;
}

GameCode LoadGameCode()
{
    bool success = CopyFileA("Game.dll", "Game_tmp.dll", false);
    if (!success) {
        printf("Failed to copy DLL: '%i'\n", GetLastError());
    }
    if (success)
        printf("Success to copy DLL\n");

    HINSTANCE dll = LoadLibraryA("Game_tmp.dll");
    assert(dll != NULL);

    tGameUpdateAndRender GameUpdateAndRender = (tGameUpdateAndRender)GetProcAddress(dll, "GameUpdateAndRender");
    assert(GameUpdateAndRender != NULL);

    tGameInit GameInit = (tGameInit)GetProcAddress(dll, "GameInit");
    assert(GameInit != NULL);

    tGameInitAfterReload GameInitAfterReload  = (tGameInitAfterReload)GetProcAddress(dll, "GameInitAfterReload");
    assert(GameInitAfterReload != NULL);

    auto write_time = last_write_time("Game.dll");

    return GameCode {
        dll,
        GameUpdateAndRender,
        GameInit,
        GameInitAfterReload,
        write_time
    };
}

void UnloadGameCode(GameCode* game)
{
    FreeLibrary(game->dll);
    game->dll = 0;
}

bool HotReloadGameCode(GameCode* game)
{
    auto dll_last_written = last_write_time("Game.dll");
    bool dll_was_changed = dll_last_written > game->dll_change_time;

    if (dll_was_changed) {
        printf("\n\nDLL CHANGED!\n\n");
        UnloadGameCode(game);
        *game = LoadGameCode();
        return true;
    }

    return false;
}
