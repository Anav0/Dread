#ifndef WEAPONS_h
#define WEAPONS_h

#pragma once

#include "GameState.h";
#include <string>;

WeaponSystem GetBmp1()
{
    WeaponSystem bmp;
    bmp.name = "BMP-1";
    bmp.image_pos_on_atlas = 0;
    bmp.cost_in_dollars = 50'000;

    bmp.attack = 2;
    bmp.defence = 1;
    bmp.state = 0.7;

    return bmp;
}

WeaponSystem GetT72()
{
    WeaponSystem vehicle;
    vehicle.name = "T72 Twardy";
    vehicle.image_pos_on_atlas = 0;
    vehicle.cost_in_dollars = 100'000;

    vehicle.attack = 4;
    vehicle.defence = 2;
    vehicle.state = 0.85;

    return vehicle;
}

#endif