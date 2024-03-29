#pragma once

#include "Engine/Misc.h"

constexpr float CARD_BASE_SCALE = 0.65;
constexpr float CARD_W = 249 * CARD_BASE_SCALE;
constexpr float CARD_H = 357 * CARD_BASE_SCALE;

constexpr AtlasTextureInfo END_TURN_ICON {
    v2(0.0, 300.0), //Pos
    v2(82.0, 82.0),  //Size
    v2(1.0) 		   // Scale
};
constexpr AtlasTextureInfo UKRAINE_EMBLEM {
    v2(0.0, 512.0), //Pos
    v2(96.0, 134.0),  //Size
    v2(1.0) 		   // Scale
};
