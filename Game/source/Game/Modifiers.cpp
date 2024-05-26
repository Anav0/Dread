#include "Modifiers.h"

void ModifiersManager::LoadWeatherModifiers(const char* path)
{
    std::ifstream file(path);

    if (!file.is_open()) {
        printf("Failed to read weather modifiers file\n");
        assert(false);
    }

    std::string line;

    getline(file, line);

    while (getline(file, line)) {
        if (line == "")
            continue;

        auto parts = split(line, ';');

        std::string& condition_str = parts[0];

        WeaponSystemGeneralType weapon_type = StrToWeaponType(parts[2]);
        f32 attack_mod = std::stof(parts[3]);
        f32 defence_mod = std::stof(parts[4]);

        if (condition_str == "Ground") {
            GroundCondition ground_condition = StrToGoundCondition(parts[1]);
            ground_condition_modifiers[ground_condition].insert({ weapon_type, { attack_mod, defence_mod } });
        }
        if (condition_str == "Air") {
            Weather weather = StrToWeather(parts[1]);
            weather_modifiers[weather].insert({ weapon_type, { attack_mod, defence_mod } });
        }
    }
}

f32 ModifiersManager::GetWeatherModifier(Weather weather, WeaponSystemGeneralType type, SideStatus status) const
{
    if (!this->weather_modifiers.contains(weather))
        return 1.0;

    if (!this->weather_modifiers.at(weather).contains(type))
        return 1.0;

    if (status == SideStatus::Attacking)
        return this->weather_modifiers.at(weather).at(type).attack_modifier;
    else
        return this->weather_modifiers.at(weather).at(type).defense_modifier;
}

f32 ModifiersManager::GetGroundConditionModifier(GroundCondition ground_condition, WeaponSystemGeneralType type, SideStatus status) const
{
    if (!this->ground_condition_modifiers.contains(ground_condition))
        return 1.0;

    if (!this->ground_condition_modifiers.at(ground_condition).contains(type))
        return 1.0;

    if (status == SideStatus::Attacking)
        return this->ground_condition_modifiers.at(ground_condition).at(type).attack_modifier;
    else
        return this->ground_condition_modifiers.at(ground_condition).at(type).defense_modifier;
}

u16 ApplySideModifier(const SideStatus status, const Modifier& modifier, u16 damage) {
    if (status == SideStatus::Attacking) {
        return damage * modifier.attack_modifier;
    } else {
        return damage * modifier.defense_modifier;
    }
}

u16 ApplyModifiers(const Side side, const SimulationParams& params, u16 damage)
{
    if (side == params.attacking_side) {
        auto modifier = params.attacking_side == Side::RU ? params.modifiers_manager.ru_modifier : params.modifiers_manager.ua_modifier;
        return ApplySideModifier(SideStatus::Attacking, modifier, damage);
    } else {
        auto modifier = params.defending_side == Side::RU ? params.modifiers_manager.ru_modifier : params.modifiers_manager.ua_modifier;
        return ApplySideModifier(SideStatus::Defending, modifier, damage);
    }

    assert(false);
}
