#ifndef WEAPONS_h
#define WEAPONS_h

#pragma once

#include "GameState.h";
#include <string>;

WeaponSystem GetBmp1()
{
    WeaponSystem vehicle;
	vehicle.type = WeaponSystemType::BMP1;
    vehicle.name = "BMP-1";
    vehicle.image_pos_on_atlas = 0;
    vehicle.cost_in_dollars = 50'000;

    vehicle.attack = 2;
    vehicle.defence = 1;
    vehicle.state = 0.7;

    return vehicle;
}

WeaponSystem GetT72()
{
    WeaponSystem vehicle;
	vehicle.type = WeaponSystemType::Twardy;
    vehicle.name = "T72 Twardy";
    vehicle.image_pos_on_atlas = 0;
    vehicle.cost_in_dollars = 100'000;

    vehicle.attack = 4;
    vehicle.defence = 2;
    vehicle.state = 0.85;

    return vehicle;
}

#endif