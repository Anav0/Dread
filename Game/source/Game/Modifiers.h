#ifndef MODIFIER_H
#define MODIFIER_H

#pragma once

//#include "Devices.h"
//#include "Weather.h"

#include <map>
#include <string>

enum class WeaponSystemGeneralType;
enum class SideStatus;
enum class Side;
enum class Weather;
enum class GroundCondition;

class SimulationParams;

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
    std::map<Weather, WeaponTypeModifierMap>         weather_modifiers;
    std::map<GroundCondition, WeaponTypeModifierMap> ground_condition_modifiers;

public:
    Modifier ru_modifier = Modifier(1.0, 1.0);
    Modifier ua_modifier = Modifier(1.0, 1.0);

    void LoadWeatherModifiers(const char* path);

    f32 GetWeatherModifier(Weather, WeaponSystemGeneralType, SideStatus) const;
    f32 GetGroundConditionModifier(GroundCondition, WeaponSystemGeneralType, SideStatus) const;
};
class TargetingInfo;
bool ArmorWasPenetrated(TargetingInfo&);

#endif
