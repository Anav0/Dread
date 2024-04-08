#pragma once

#include <vector>
#include <string>

#include "Engine/Constants.h"

enum class UnitSize {
	Division,
	Brigade,
	Regiment,
	Battalion,
	Company,
	Platoon,
	Squad,
	Teram,
};

enum class WeaponDomain {
	Cyber,
	Ground,
	Air,
	Sea,
};

enum class WeaponSystemType {
	Infantry,
	Twardy,
	BMP1,
	BMP2,
	BMP3,
	T74,
	T64,
	T62,
	T74B3,
	T90,
	Bradley,
	LEO2A3,
	LEO2A4,
	LEO2A5,
};

struct WeaponSystem {
	WeaponSystemType type;
	WeaponDomain domain;

	std::string name;
	u16 image_pos_on_atlas;
	u32 cost_in_dollars;

	u8 AA;
	u8 attack;
	u8 defence;
	f32 state = 1.0;
};

struct Commander {
	char* name;
};

struct Unit {
	std::string name;
	UnitSize size;
	Commander commander;

	std::vector<WeaponSystem> weapons;
	std::vector<u16>          weapons_counter;
};

struct Fight {
	std::vector<Unit> ukr_units;
	std::vector<Unit> ru_units;
};
