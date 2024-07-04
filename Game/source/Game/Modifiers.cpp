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

// TODO: Implement
HitDirection DeterminHitDirection(TargetingInfo& targeting_info)
{
    return HitDirection::HullFront;
}

bool ArmorWasPenetrated(TargetingInfo& targeting_info)
{
    HitDirection dir = DeterminHitDirection(targeting_info);
    u32 armor_value = targeting_info.targeted_weapon->weapon->GetArmorAt(dir);
    return armor_value < targeting_info.ammo_to_use->penetration;
}
