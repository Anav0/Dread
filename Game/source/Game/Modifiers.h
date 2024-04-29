#pragma once

#include "Devices.h"
#include "Weather.h"
#include "Fight.h"

#include <map>
#include <string>

struct Modifier {
    f32 defense_modifier = 1.0;
    f32 attack_modifier = 1.0;

    Modifier(f32 attack_modifier, f32 defense_modifier)
        : defense_modifier(defense_modifier)
        , attack_modifier(attack_modifier)
    {
    }
};

typedef std::map<WeaponSystemGeneralType, Modifier> WeaponTypeModifierMap;

class ModifiersManager {
private:
    //std::map<Weather, WeaponTypeModifierMap> weather_modifiers;
    //std::map<GroundCondition, WeaponTypeModifierMap> ground_condition_modifiers;

public:
    void LoadWeatherModifiers(const char* path);
};
