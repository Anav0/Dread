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