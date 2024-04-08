#pragma once

#include <vector>
#include <string>
#include <random>
#include <set>

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

enum class Armor {
	Soft,
	Hard,
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
	Gozdik,
};

struct Accuracy {
	u32 range_in_meters;
	//NOTE: change to hit stationary target
	f32 change_to_hit;
};

struct Ammo {
	std::string name;
	WeaponDomain domain;
	u16 soft;
	u16 hard;
	u16 AA;
	std::vector<Accuracy> accuracy;
};

struct Device {
	std::string name;  
	std::set<u32> ammunition;
};  

struct WeaponSystem {
	WeaponSystemType type;
	Armor armor;

	std::string name;
	u32 speed;
	f32 state = 1000.0;

	u16 image_pos_on_atlas;
	u32 cost_in_dollars;

	std::vector<u32> devices;
};

struct Commander {
	char* name;
};

struct Unit {
	std::string name;
	UnitSize size;
	Commander commander;

	std::vector<WeaponSystem> weapons;
	std::vector<f32>          morale;
	std::vector<u16>          weapons_counter;
};

struct BattleGroup {
	std::string name;
	u32 unit_index;

	std::vector<f32>          morale;
	std::vector<WeaponSystem> weapons;
	std::vector<u16>          weapons_counter;
};

enum class Side {
	UA,
	RU
};

enum class UnitStance {
	Reserve,
	Committed,
	Retreating,
	Redeployed,
	Defending,
};

constexpr u8 MAX_UNITS = 24;
constexpr u8 SUPPORT_ASSETS = 8;

struct Fight {
	u8 phase = 0;

	u16 attacker_distance_in_meters;

	//NOTE: -1 means slot is free
	i32 ua_units[MAX_UNITS];
	i32 ru_units[MAX_UNITS];

	i32 ua_enabling_assets[SUPPORT_ASSETS];
	i32 ru_enabling_assets[SUPPORT_ASSETS];

	UnitStance ua_stance[MAX_UNITS];
	UnitStance ru_stance[MAX_UNITS];

	void SimulateAttack();
  std::vector<BattleGroup> FormUAGroup(UnitStance);
  std::vector<BattleGroup> FormRUGroup(UnitStance);

	Fight() {
		for(u8 i = 0; i < MAX_UNITS; i++) {
				ua_units[i] = -1;
				ru_units[i] = -1;
		}
	}

};

void TestFight();

bool MoralBroke(std::vector<BattleGroup>& groups);
bool AttackerSufferedHeavyLosses(std::vector<BattleGroup>& groups);
bool TryToHitTarget(u32 distance, Device& device);

void Fire(u32 distance_in_m, std::vector<BattleGroup>& fire, std::vector<BattleGroup>& target);
