#pragma once

#include "Fight.h"

struct Armory {
    std::vector<WeaponSystem> weapons;
    std::vector<Device> devices;
    std::vector<Ammo> ammo;

    std::vector<u32> ua_ammo_quantity;
    std::vector<u32> ua_weapons_quantity;

    std::vector<u32> ru_ammo_quantity;
    std::vector<u32> ru_weapons_quantity;
};

void PrintArmory(Armory& armory);
Armory LoadArmory(const char* path, const char* storage_path);
