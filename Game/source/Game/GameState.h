#pragma once

#ifndef GameState_H
#define GameState_H

#include "Entities.h"

#include "Engine/Camera.h"
#include "Engine/Constants.h"
#include "Engine/WindowManager.h"

#include <array>
#include <string>
#include <set>
#include <map>

constexpr static u8 NUMBER_OF_OBLASTS    = 25;
constexpr static u8 NUMBER_OF_SUPPORTERS = 10;

constexpr bool DEBUG_LINES = false;
constexpr bool CULLING     = true;

constexpr const char* MONTHS[] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

constexpr const char* PHASES[] = {
	"So it begins",
	"Events",
	"Player action",
	"Main phase",
	"Russia",
};

enum class GamePhase {
	Inital,
	Events,
	AllocateAssets,
	Resolution,
	CPU,
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

struct Delivery {
	u16 n;
	WeaponSystem system;
};

struct SupportPackage {
	std::string name;
	u32 delivered_on_turn;
	CountryCode origin;
	bool fully_delivered = false;
	std::vector<Delivery> delivery;
};

//------------------------------------------------------------------------

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

struct Deployment {
	std::vector<Unit>       ukr_units;
	std::vector<OblastCode> ukr_assigned;
	std::vector<Unit>       ru_units;
	std::vector<OblastCode> ru_assigned;
};

struct GameState {
	std::string game_date_str;

	OblastCode selected_oblast;
	GamePhase phase = GamePhase::Inital;

	f32 popular_support = 0.8;
	u64 reserve         = 600'000;

	std::vector<Country> countries;

	std::vector<SupportPackage> promised_support;
	std::vector<Delivery>       weapons_in_reserve;

	Deployment troops_deployment;

	u8 current_turn = 1;
	u8 max_turn     = 12 * 3; //Three years, 12 months per year 3 years

	bool turn_changed = false;

	void Reset();
};

const char* GetMonth();
void GetDateStr(std::string& date_str);
std::string GetYear();

Oblast* GetOblast(OblastCode code);

void ChangeControl(f32 by);
void GoToNextPhase();
void GoToNextTurn();
void ComputerPhase();
void DrawEndScreen();
void PromiseSupport(SupportPackage);
Country& GetCountryByCode(CountryCode code);
Delivery* GetDeliveryByType(WeaponSystemType type);

extern GameState STATE;

WeaponSystem GetBmp1()
{
    WeaponSystem vehicle;
		vehicle.domain = WeaponDomain::Ground;
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
		vehicle.domain = WeaponDomain::Ground;
    vehicle.name = "T72 Twardy";
    vehicle.image_pos_on_atlas = 0;
    vehicle.cost_in_dollars = 100'000;

    vehicle.attack = 4;
    vehicle.defence = 2;
    vehicle.state = 0.85;

    return vehicle;
}

#endif
