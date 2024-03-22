#include "Entities.h"

void Oblast::UpdateColorBasedOnControl()
{
    v4 color = lerp(RUSSIAN_COLOR, UKRAINE_COLOR, ukrainian_control);
    mesh.ChangeColor(color);
}

void Oblast::UpdateColorBasedOnFactor(FactorToShow factor) {
	u32 total_ukrainian_air_power_in_oblast = GetUkrainianFactor(factor);
	u32 total_russian_air_power_in_oblast   = GetRussianFactor(factor);
	f32 v = static_cast<f32>(total_ukrainian_air_power_in_oblast) / total_russian_air_power_in_oblast;
	v4 color = lerp(RED, BLUE, v);
  mesh.ChangeColor(color);
}

u32 Oblast::GetUkrainianFactor(FactorToShow factor) {
	return 64;
}

u32 Oblast::GetRussianFactor(FactorToShow factor) {
	return 84;
}
