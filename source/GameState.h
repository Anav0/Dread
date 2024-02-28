#pragma once

#ifndef GameState_H
#define GameState_H

#include "Camera.h"
#include "Constants.h"
#include "Entities.h"
#include "WindowManager.h"

#include <array>
#include <string>
#include <set>
#include <map>

enum class RenderMode {
    WIREFRAME,
    NORMAL,
};

constexpr static u8 NUMBER_OF_OBLASTS    = 25;
constexpr static u8 NUMBER_OF_SUPPORTERS = 10;

constexpr bool HIDE_ALPHA  = false;
constexpr bool DEBUG_LINES = false;
constexpr bool DEBUG       = true;
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

enum class WeaponSystemType {
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
	std::string name;
	u16 image_pos_on_atlas;
	u32 cost_in_dollars;

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

struct GameState {
  WindowManager window;
  RenderMode mode = RenderMode::NORMAL;
  bool show_collisions = false;

  OblastCode selected_oblast;

  f32 popular_support = 0.8;
  u64 reserve         = 600'000;

	std::vector<Country> countries;

	std::vector<SupportPackage> promised_support;
	std::vector<Delivery>       weapons_in_reserve;

	u8 current_turn = 1;
	u8 max_turn     = 12 * 3; //Three years, 12 months per year 3 years

	bool turn_changed = false;

	void Reset();
};

const char* GetMonth();
std::string GetDateStr();
std::string GetYear();

Oblast* GetOblast(OblastCode code);

v2 GetScreenSize();
void ChangeControl(f32 by);
void GoToNextPhase();
void GoToNextTurn();
void ComputerPhase();
void DrawEndScreen();
void PromiseSupport(SupportPackage);
Country& GetCountryByCode(CountryCode code);
Delivery* GetDeliveryByType(WeaponSystemType type);

extern GameState STATE;

#endif
