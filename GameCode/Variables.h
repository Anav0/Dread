#pragma once

#ifdef GAMECODE_EXPORTS
#define GAMECODE_API __declspec(dllexport)
#else
#define GAMECODE_API __declspec(dllimport)
#endif

constexpr int DLL_TEST = 255;

extern "C" GAMECODE_API void GameUpdateAndRender();
