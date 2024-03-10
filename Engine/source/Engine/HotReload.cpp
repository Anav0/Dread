#include "HotReload.h"

// Windows
#include <cassert>
#include <stdio.h>
#include <windows.h>
#include <winnt.rh>

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

    tGameInitEx GameInitEx = (tGameInitEx)GetProcAddress(dll, "GameInitEx");
    assert(GameInitEx != NULL);

    tGameInitAfterReload GameInitAfterReload = (tGameInitAfterReload)GetProcAddress(dll, "GameInitAfterReload");
    assert(GameInitAfterReload != NULL);

    auto write_time = last_write_time("Game.dll");

    return GameCode {
        dll,
        GameUpdateAndRender,
        GameInit,
        GameInitEx,
        GameInitAfterReload,
        write_time
    };
}

void UnloadGameCode(GameCode* game)
{
    FreeLibrary(game->dll);
    game->dll = 0;
}

bool GameCodeChanged(GameCode* game)
{
    auto dll_last_written = last_write_time("Game.dll");
    bool dll_was_changed = dll_last_written > game->dll_change_time;

    return dll_was_changed;
}
