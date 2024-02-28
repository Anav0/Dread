#include "pch.h"
#include "GameCode.h"

void GameUpdateAndRender()
{
}

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
