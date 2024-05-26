#ifndef ENGINE_COLORS_H
#define ENGINE_COLORS_H

#pragma once

#include "GLM.h"
#include "Misc/Constants.h"

constexpr v4 TRANS   = v4(0.0f, 0.0f, 0.0f, 0.0f);

constexpr v4 RUSSIAN_COLOR = v4(0.96f, 0.34f, 0.38f, 1.0f);
constexpr v4 UKRAINE_COLOR = v4(0.16f, 0.41f, 0.75f, 1.0f);

constexpr v4 YELLOW = v4(0.96f, 0.76f, 0.47f, 1.0f);
constexpr v4 BLUE   = v4(0.34f, 0.61f, 0.78f, 1.0f);
constexpr v4 RED    = v4(0.58f, 0.03f, 0.01f, 1.0f);
constexpr v4 WHITE  = v4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr v4 BLACK  = v4(0.0f, 0.0f, 0.0f, 1.0f);
constexpr v4 GOLD   = v4(0.92f, 0.71f, 0.23f, 1.0f);
constexpr v4 GREY   = v4(0.5f, 0.5f, 0.5f, 1.0f);

//FACTORS

constexpr v4 UI_BTN_BG = GREY;
constexpr v4 UI_BTN_HOVER_BG = GREY;

#endif
