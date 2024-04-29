#pragma once

#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "Engine/Constants.h"
#include <Misc/CsvSaver.h>

#include "Entities.h"

class Modifier;

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

enum class WeaponSystemGeneralType {
    Infantry,
    APC,
    IFV,
    Artillery,
    MLRS,
    Vehicle,
    Drone,
    Tank,
    MANPADS,
    ATGM,
};

struct Accuracy {
    u32 range_in_meters;
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
    Armor armor;
    WeaponDomain domain;
    WeaponSystemGeneralType type;

    f32 default_state;

    std::string name;

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

struct Armory {
    std::vector<WeaponSystem> weapons;
    std::vector<Device> devices;
    std::vector<Ammo> ammo;

    std::vector<u32> ua_ammo_quantity;
    std::vector<u32> ua_weapons_quantity;

    std::vector<u32> ru_ammo_quantity;
    std::vector<u32> ru_weapons_quantity;

    std::optional<u32> GetWeaponIndexByName(const std::string& name);

    Armory() { }
    Armory(Armory& armory)
    {
        this->ammo = armory.ammo;
        this->weapons = armory.weapons;
        this->devices = armory.devices;
        this->ua_ammo_quantity = armory.ua_ammo_quantity;
        this->ua_weapons_quantity = armory.ua_weapons_quantity;
        this->ru_ammo_quantity = armory.ru_ammo_quantity;
        this->ru_weapons_quantity = armory.ru_weapons_quantity;
    }
};

struct Deployment {
    std::vector<Unit> ukr_units;
    std::vector<OblastCode> ukr_assigned;
    std::vector<Unit> ru_units;
    std::vector<OblastCode> ru_assigned;

    Deployment() { }
    Deployment(Deployment& deployment)
    {
        this->ukr_units = deployment.ukr_units;
        this->ukr_assigned = deployment.ukr_assigned;
        this->ru_units = deployment.ru_units;
        this->ru_assigned = deployment.ru_assigned;
    }
};

void PrintUnit(Armory& armory, Unit& unit);

struct RoundInfo {
    // Armory* armory;
    u32 round;
    u32 run;
    u32 distance;

    RoundInfo() { }

    RoundInfo(Armory* armory)
    {
    }

    std::string ToCsvRow() const
    {
        std::stringstream ss;

        ss << run << ";";
        ss << round << ";";
        ss << distance << "";

        return ss.str();
    }
};

std::string ArmorToStr(Armor armor);
std::string DomainToStr(WeaponDomain domain);

std::string WeaponTypeToStr(WeaponSystemGeneralType type);

struct BattleGroup {
    std::string name;
    u32 parent_unit_index;

    std::vector<WeaponSystem*> weapons;
    std::vector<f32> morale;
    std::vector<u16> weapons_counter;
    std::vector<f32> weapons_state;
    std::vector<f32> initial_weapons_state;

    RoundInfo round_info;
    u32 Attacking;
    std::string Side;
    u32 GroupIndex;
    std::string Domain;

    BattleGroup(Armory* armory)
    {
        round_info = RoundInfo(armory);
    }

    std::string ToCsvRow() const
    {
        std::stringstream ss;

        auto round_info_str = round_info.ToCsvRow();

        u32 index = 0;
        u32 individual_index = 0;
        for (auto& w : weapons) {
            for (u32 i = 0; i <= weapons_counter.at(index); i++) {
                for (auto& device_index : w->devices) {
                    // Device& device = round_info.armory->devices.at(device_index);
                    ss << round_info_str << ";";
                    ss << Attacking << ";";
                    ss << Side << ";";
                    ss << GroupIndex << ";";
                    ss << Domain << ";";
                    ss << w->name << ";";
                    ss << WeaponTypeToStr(w->type) << ";";
                    ss << individual_index << ";";
                    ss << weapons_state.at(index) << ";";
                    ss << ArmorToStr(w->armor)
                       << "\n";
                }
                individual_index++;
            }
            index++;
        }

        return ss.str();
    }
};

enum class SideStatus {
    Attacking,
    Defending,
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

WeaponSystemGeneralType StrToWeaponType(std::string& str);

struct FireResult {
    u64 run;
    u64 round;
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
        ss << run << ";"
           << round << ";"
           << firing_side << ";"
           << status << ";"
           << firing_weapons_system->name << ";"
           << WeaponTypeToStr(firing_weapons_system->type) << ";"
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

std::string SideToStr(Side side);

struct SimulationSession {
    u32 run;
    u32 round;
    u32 distance_in_meters;

    Armory* armory;
    Deployment& deployment;

    CsvSaver round_saver, battle_groups_saver, fight_result_saver;

    std::set<std::string>& save_only_this;

    const std::string GROUPS = "groups";
    const std::string RESULT = "result";
    const std::string FIRING = "firing";

    bool ShouldSaveFiring() const
    {
        bool have = save_only_this.contains(FIRING);
        return have;
    }

    bool ShouldSaveGroups() const
    {
        bool have = save_only_this.contains(GROUPS);
        return have;
    }

    bool ShouldSaveResult() const
    {
        bool have = save_only_this.contains(RESULT);
        return have;
    }

    SimulationSession(Armory* armory, Deployment& deployment, std::set<std::string>& save_only_this)
        : armory(armory)
        , deployment(deployment)
        , round(0)
        , run(0)
        , distance_in_meters(0)
        , save_only_this(save_only_this)
    {
        if (ShouldSaveGroups()) {
            battle_groups_saver = CsvSaver("battle_groups.csv");
            battle_groups_saver.AddHeader("Run;Round;Distance;Attacking;Side;GroupIndex;WeaponDomain;Weapon;WeaponType;WeaponIndex;State;Armor");
        }
        if (ShouldSaveFiring()) {

            round_saver = CsvSaver("data.csv");
            round_saver.AddHeader("Run;Round;Side;Status;Weapon;Type;Device;ACC;TargetWeapon;StartingState;Dmg;StateAfterHit;Morale;MoraleAfterHit;Distance");
        }
        if (ShouldSaveResult()) {
            fight_result_saver = CsvSaver("result.csv");
            fight_result_saver.AddHeader("Run;WhoWon");
        }
    };

    void AddWinner(Side winner)
    {
        if (!ShouldSaveResult())
            return;

        std::string str = std::to_string(this->run) + ";" + SideToStr(winner);
        fight_result_saver.AddRowRaw(str);
    }

    void AddGroup(std::vector<BattleGroup>& groups)
    {
        if (!ShouldSaveGroups())
            return;

        for (auto& group : groups) {
            group.round_info.run = run;
            group.round_info.round = round;
            group.round_info.distance = distance_in_meters;
            battle_groups_saver.AddRow(group);
        }
    }

    void AddRound(std::vector<FireResult>& rounds, const std::string& side)
    {
        if (!ShouldSaveFiring())
            return;

        for (FireResult& r : rounds) {
            r.firing_side = side;
            r.round = round;
            r.run = run;
            round_saver.AddRow(r);
        }
    }
    void AddRound(std::vector<FireResult>& rounds, const char* side)
    {
        if (!ShouldSaveFiring())
            return;

        for (FireResult& r : rounds) {
            r.firing_side = side;
            r.round = round;
            r.run = run;
            round_saver.AddRow(r);
        }
    }

    void Flush()
    {
        if (ShouldSaveFiring())
            round_saver.Flush();
        if (ShouldSaveGroups())
            battle_groups_saver.Flush();
        if (ShouldSaveResult())
            fight_result_saver.Flush();
    }

    void Close()
    {
        if (ShouldSaveFiring())
            round_saver.Close();
        if (ShouldSaveGroups())
            battle_groups_saver.Close();
        if (ShouldSaveResult())
            fight_result_saver.Close();
    }
};

struct SimulationParams {
    Side attacking_side;
    Side defending_side;

    const Modifier& ua_modifiers;
    const Modifier& ru_modifiers;

    SimulationParams(const Side& attackingSide, const Modifier& ua_modifier, const Modifier& ru_modifier)
        : attacking_side(attackingSide)
        , defending_side(attackingSide == Side::RU ? Side::UA : Side::RU)
        , ua_modifiers(ua_modifier)
        , ru_modifiers(ru_modifier)
    {
    }
};

struct Fight {
    u16 attacker_distance_in_meters;

    // NOTE: -1 means slot is free
    i32 ua_units[MAX_UNITS];
    i32 ru_units[MAX_UNITS];

    i32 ua_enabling_assets[SUPPORT_ASSETS];
    i32 ru_enabling_assets[SUPPORT_ASSETS];

    UnitStance ua_stance[MAX_UNITS];
    UnitStance ru_stance[MAX_UNITS];

    void SimulateAttack(SimulationParams&, Armory*, Deployment&, SimulationSession&);

    std::vector<BattleGroup> FormBattleGroups(Side side, Armory* armory, UnitStance stance, Deployment& deployment);

    Fight()
    {
        for (u8 i = 0; i < MAX_UNITS; i++) {
            ua_units[i] = -1;
            ru_units[i] = -1;
        }
    }
};

BattleGroup FormBattleGroup(Armory* armory, u32 parent_unit_index, Unit& unit);

f32 GetModifier(SimulationParams& params, SideStatus status);
bool MoralBroke(std::vector<BattleGroup>& groups, f32 threshold);
bool AverageDamageExceedsThreshold(std::vector<BattleGroup>& groups, f32 threshold);
std::tuple<bool, f32> TryToHitTarget(Ammo& ammo, u32 distance);
std::vector<FireResult> Fire(Armory* armory, const f32 modifier, u16 distance_in_m, const std::vector<BattleGroup>& attacking_battlegroups, std::vector<BattleGroup>& targeted_battlegroups);
