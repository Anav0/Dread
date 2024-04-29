#pragma once

#ifndef GameState_H
#define GameState_H

#include "Devices.h"
#include "Entities.h"
#include "Fight.h"
#include "Weather.h"
#include "Modifiers.h"

#include "Engine/Camera.h"
#include "Engine/Constants.h"
#include "Engine/WindowManager.h"

#include <array>
#include <map>
#include <set>
#include <string>

constexpr static u8 NUMBER_OF_OBLASTS = 25;
constexpr static u8 NUMBER_OF_SUPPORTERS = 10;

constexpr bool DEBUG_LINES = false;
constexpr bool CULLING = true;

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
    std::string game_date_str;

    OblastCode selected_oblast;
    GamePhase phase = GamePhase::Inital;

    f32 popular_support = 0.8;
    u64 reserve = 600'000;

    std::vector<Country> countries;

    std::vector<SupportPackage> promised_support;
    std::vector<Delivery> weapons_in_reserve;

    Modifier ua_modifier = Modifier(1.05, 1.5);
    Modifier ru_modifier = Modifier(0.8, 1.0);

    Armory armory;

    std::vector<Commander> commanders;

    Deployment troops_deployment;
    WeatherManager weather_manager;

    u8 current_turn = 1;
    u8 max_turn = 12 * 3; // Three years, 12 months per year 3 years

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
Delivery* GetDeliveryByType(WeaponSystemGeneralType type);

extern GameState STATE;

#endif
