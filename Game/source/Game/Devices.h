#pragma once

#include "Fight.h"

struct Armory {
    std::vector<WeaponSystem> weapons;
    std::vector<Device> devices;
    std::vector<Ammo> ammo;
};

void PrintArmory(Armory& armory);
Armory LoadArmory(const char* path);
