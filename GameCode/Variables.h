#pragma once

#ifdef GAMECODE_EXPORTS
#define GAMECODE_API __declspec(dllexport)
#else
#define GAMECODE_API __declspec(dllimport)
#endif

#include "Constants.h";
#include <array>

enum class GradientType : i32 {
	Radial = 0,
	ThreeColor = 1,
};

struct Gradient {
	GradientType gradient_type;
	v2 middle;
	v2 radial_position;
	f32 radial_factor;
	std::array<v4, 4> colors;
};

typedef void (*tGameUpdateAndRender) (void);

extern "C" const GAMECODE_API Gradient card_gradient {
		GradientType::Radial,
		v2(0.5, 0.5),
		{0.777, -0.56},
		1.0,
		{YELLOW, BLACK},
};

extern "C" GAMECODE_API void GameUpdateAndRender();
