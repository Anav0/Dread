#pragma once

#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "Engine/Constants.h"
#include "Entities.h"
#include <Misc/CsvSaver.h>

class Armory;

enum class UnitSize {
    Division,
    Brigade,
    Regiment,
    Battalion,
    Company,
    Platoon,
    Squad,
};

const std::map<std::string, UnitSize> STR_TO_SIZE = {
    { "Division", UnitSize::Division },
    { "Brigade", UnitSize::Brigade },
    { "Regiment", UnitSize::Regiment },
    { "Battalion", UnitSize::Battalion },
    { "Company", UnitSize::Company },
    { "Platoon", UnitSize::Platoon },
    { "Squad", UnitSize::Squad },
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
    // NOTE: change to hit stationary target
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
    WeaponDomain domain;

    std::string name;
    f32 state = 1000.0;
    f32 inital_state = 1000.0;

    u16 image_pos_on_atlas;
    u32 cost_in_dollars;

    std::vector<u32> devices;
};

enum class Side {
    UA,
    RU
};

struct Commander {
    char* name;
};

struct Unit {
    std::string name;
    std::string nickname;
    UnitSize size;
    Side side;
    u32 commander_index;

    std::vector<u32> weapons;
    std::vector<f32> morale;
    std::vector<u16> weapons_counter;
    std::vector<u16> weapons_toe;
};

struct Deployment {
    std::vector<Unit> ukr_units;
    std::vector<OblastCode> ukr_assigned;
    std::vector<Unit> ru_units;
    std::vector<OblastCode> ru_assigned;
};

void PrintUnit(Unit& unit);

struct BattleGroup {
    std::string name;
    u32 parent_unit_index;

    std::vector<WeaponSystem*> weapons;
    std::vector<f32> morale;
    std::vector<u16> weapons_counter;
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

// # Iter FiringSide Status Weapon Device ACC TargetWeapon StartingState Dmg StateAfterHit Distance
// 0    UA HIT  BMP2   2A42   0.5 BMP1 100 24 76 2400
struct FireResult {
    u64 iter;
    std::string firing_side;
    std::string status;
    Device& firing_device;
    WeaponSystem* firing_weapons_system;
    WeaponSystem* targeted_weapons_system;
    f32 acc;
    f32 starting_state;
    f32 dmg;
    f32 state_after_damage;
    f32 morale;
    f32 morale_after_damage;
    u32 distance;

    FireResult(Device& firing_device, WeaponSystem* firing_weapons_system, WeaponSystem* targeted_weapons_system)
        : firing_weapons_system(firing_weapons_system)
        , firing_device(firing_device)
        , targeted_weapons_system(targeted_weapons_system) {};

    std::string ToCsvRow() const
    {
        std::stringstream ss;
        ss << iter << ";"
           << firing_side << ";"
           << status << ";"
           << firing_weapons_system->name << ";"
           << firing_device.name << ";"
           << acc << ";"
           << targeted_weapons_system->name << ";"
           << starting_state << ";"
           << dmg << ";"
           << state_after_damage << ";"
           << morale << ";"
           << morale_after_damage << ";"
           << distance;

        return ss.str();
    }
};

struct SimulationSession {
    u64 iter;
    Armory& armory;
    Deployment& deployment;
    std::vector<FireResult> rounds;

    SimulationSession(Armory& armory, Deployment& deployment)
        : armory(armory)
        , deployment(deployment) {};
};

struct Fight {
    CsvSaver* saver;

    u8 phase = 0;

    u16 attacker_distance_in_meters;

    // NOTE: -1 means slot is free
    i32 ua_units[MAX_UNITS];
    i32 ru_units[MAX_UNITS];

    i32 ua_enabling_assets[SUPPORT_ASSETS];
    i32 ru_enabling_assets[SUPPORT_ASSETS];

    UnitStance ua_stance[MAX_UNITS];
    UnitStance ru_stance[MAX_UNITS];

    void SimulateAttack(Armory&, Deployment&);
    std::vector<BattleGroup> FormUAGroup(Armory&, UnitStance, Deployment&);
    std::vector<BattleGroup> FormRUGroup(Armory&, UnitStance, Deployment&);

    Fight()
    {
        for (u8 i = 0; i < MAX_UNITS; i++) {
            ua_units[i] = -1;
            ru_units[i] = -1;
        }
    }
};

bool MoralBroke(std::vector<BattleGroup>& groups);
bool AverageDamageExceedsThreshold(std::vector<BattleGroup>& groups, f32 threshold);
std::tuple<bool, f32> TryToHitTarget(Ammo& ammo, u32 distance);
std::vector<FireResult> Fire(Armory& armory, u32 distance_in_m, std::vector<BattleGroup>& fire, std::vector<BattleGroup>& target);
