#pragma once

#include "Constants.h";
#include <array>

#define EXPORT extern "C" __declspec(dllexport)

constexpr u8 GRADIENT_MAX_COLORS = 4;

enum class GradientType : i32 {
	Radial = 0,
	ThreeColor = 1,
};

struct Gradient {
	GradientType gradient_type;
	v2 middle;
	v2 radial_position;
	f32 radial_factor;
	std::array<v4, GRADIENT_MAX_COLORS> colors;
};

typedef void (*tGameUpdateAndRender) (void);

struct Constants {
	Gradient header_gradient;
	Gradient card_gradient;
};

const Gradient card_gradient {
		GradientType::Radial,
		v2(0.5, 0.5),
		{0.777, -0.56},
		1.0,
		{YELLOW, BLACK},
};

const Gradient header_gradient {
    GradientType::ThreeColor,
    v2(0.25, 0.65),
    { 0.0, 0.0 },
    1.0,
    { RED, BLACK, YELLOW },
};

EXPORT const Constants cons {
	header_gradient,
	card_gradient,
};

EXPORT void GameUpdateAndRender();
