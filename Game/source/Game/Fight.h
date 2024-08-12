#ifndef FIGHT_H
#define FIGHT_H

#pragma once

#include <array>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "Engine/GLM.h"
#include "Misc/Constants.h"
#include "Misc/CsvSaver.h"
#include "Misc/Flags.h"

#include "Entities.h"
#include "Modifiers.h"
#include "Units.h"

class WeatherManager;

struct Armory {
    std::vector<WeaponSystem> weapons;
    std::vector<Device> devices;
    std::vector<Ammo> ammo;

    std::vector<u32> ua_ammo_quantity;
    std::vector<u32> ua_weapons_quantity;

    std::vector<u32> ru_ammo_quantity;
    std::vector<u32> ru_weapons_quantity;

    WeaponSystem* GetWeaponById(std::string_view id);
    std::optional<u32> GetWeaponIndexByName(std::string_view name);
    std::optional<u32> GetDeviceIndexById(std::string_view device_id);
    std::optional<u32> GetAmmoIndexById(std::string_view ammo_id);
    std::optional<u32> GetWeaponIndexById(std::string_view weapon_id);

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
    std::vector<Unit> units;
    std::vector<OblastCode> assigned;

    std::map<UnitId, UnitIndex> unit_id_by_unit_index;

    Deployment() { }
    Deployment(Deployment& deployment)
    {
        this->units = deployment.units;
        this->assigned = deployment.assigned;
    }
    Unit* GetUnitById(const std::string& id);
    u32 Insert(Unit, OblastCode);
};


struct RoundInfo {
    // Armory* armory;
    u32 round;
    u32 run;
    u32 distance;

    RoundInfo() { }

    RoundInfo(Armory* armory)
    {
    }

    std::string ToString() const
    {
        std::stringstream ss;

        ss << run << ";";
        ss << round << ";";
        ss << distance << "";

        return ss.str();
    }
};

struct WeaponSystemInGroup {
    u32 index_of_weapon_in_parent_unit;
    WeaponSystem* weapon;
    f32 morale;
    u32 statuses; //WeaponSystemStatus flag
};


struct Priority {
    u32 index;
    u32 priority;

    bool operator<(const Priority& other) const
    {
        return priority < other.priority;
    }

    bool operator>(const Priority& other) const
    {
        return priority > other.priority;
    }
};

// SPEED:
using PriorityQueue = std::vector<std::vector<Priority>>;

constexpr u32 MAX_BG_SIZE = 128;
struct BattleGroup {
    std::string name;
    std::string Side;

    std::array<WeaponSystemInGroup, MAX_BG_SIZE> weapons;

    u32 parent_unit_index;
    u32 real_size = 0;
    u32 Attacking;

    RoundInfo round_info;
    u32 GroupIndex;
    std::string Domain;

    BattleGroup(Armory* armory)
    {
        round_info = RoundInfo(armory);
    }

    f32 AverageMorale()
    {
        f32 sum = 0;
        for (u32 i = 0; i < real_size; i++) {
            sum += weapons[i].morale;
        }
        return sum / real_size;
    }

    void ToCsvRow(std::ostringstream& ss) const
    {
        auto round_info_str = round_info.ToString();

        u32 index = 0;
        for (u32 i = 0; i < real_size; i++) {
            auto& w = weapons[i];
            const auto& name = WeaponTypeToStr(w.weapon->type);
            const auto unit_name = w.weapon->name;

            for (auto& device_index : w.weapon->devices) {
                // Device& device = round_info.armory->devices.at(device_index);
                ss << round_info_str << ";";
                ss << Attacking << ";";
                ss << Side << ";";
                ss << GroupIndex << ";";
                ss << Domain << ";";
                ss << unit_name << ";";
                ss << name << ";";
                ss << index << ";";
                ss << StatusesToStr(w.statuses) << "\n";
            }
            index++;
        }
    }
};

enum class SideStatus {
    Attacking,
    Defending,
};

constexpr u8 MAX_UNITS = 24;
constexpr u8 SUPPORT_ASSETS = 8;

enum class AttackResultStatus {
    AttackerWon,
    DefenderWon,
    Draw,
};

const BiMap<AttackResultStatus, std::string> STR_TO_ATTACK_RESULT_STATUS = {
    { AttackResultStatus::AttackerWon, "AttackerWon" },
    { AttackResultStatus::DefenderWon, "DefenderWon" },
    { AttackResultStatus::Draw,        "Draw"        },
};

struct AttackResult {
    AttackResultStatus status;
};

struct TryToHitParams {
	std::mt19937 engine;
  std::uniform_real_distribution<f32> distribution;
};

struct TargetingInfo {
    bool can_fire;
    const BattleGroup* targeted_group;
    WeaponSystemInGroup* targeted_weapon;
    const WeaponSystemInGroup* weapon_to_use;
    const Device* device_to_use;
    const Ammo* ammo_to_use;
};

struct FireResult {
    u64 run;
    u64 round;
    std::string firing_side;
    std::string hit_or_miss;
    TargetingInfo targeting_info;
    f32 acc;
    f32 morale;
    f32 morale_after_damage;
    u32 distance;

    FireResult(TargetingInfo targeting_info)
        : targeting_info(targeting_info) {};

    // TODO:
    void ToCsvRow(std::ostringstream& ss) const
    {
        ss << "EMPTY"
           << "\n";
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

    void AddResult(AttackResultStatus status)
    {
        if (!ShouldSaveResult())
            return;

        std::string str = std::to_string(this->run) + ";" + STR_TO_ATTACK_RESULT_STATUS.GetValue(status);
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
    const OblastCode oblast_code;
    Side attacking_side;
    Side defending_side;

    const WeatherManager& weather_manager;
    const ModifiersManager& modifiers_manager;

    std::mt19937 rnd_engine;

    SimulationParams(
        const OblastCode oblast_code,
        const Side attackingSide,
        const WeatherManager& weather_manager,
        const ModifiersManager& modifiers_manager)
        : attacking_side(attackingSide)
        , oblast_code(oblast_code)
        , defending_side(attackingSide == Side::RU ? Side::UA : Side::RU)
        , weather_manager(weather_manager)
        , modifiers_manager(modifiers_manager)
    {
        std::random_device rd;
        rnd_engine = std::mt19937(rd());
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

    AttackResult SimulateAttack(SimulationParams&, Armory*, Deployment&, SimulationSession*);

    std::vector<BattleGroup> FormBattleGroups(OblastCode oblast, Side side, Armory* armory, UnitStance stance, Deployment& deployment);

    void DissolveBattleGroups(std::vector<BattleGroup>&, Deployment&);

    Fight()
    {
        for (u8 i = 0; i < MAX_UNITS; i++) {
            ua_units[i] = -1;
            ru_units[i] = -1;
        }
    }
};

inline bool UnitDestroyed(const WeaponSystemInGroup& weapon)
{
    bool is_crew_dead = HAS_FLAG(weapon.statuses, WeaponSystemStatus::DriverKilled) && HAS_FLAG(weapon.statuses, WeaponSystemStatus::GunnerKilled) && HAS_FLAG(weapon.statuses, WeaponSystemStatus::CmdrKilled);

    return HAS_FLAG(weapon.statuses, WeaponSystemStatus::OnFire)
        || HAS_FLAG(weapon.statuses, WeaponSystemStatus::Abondoned)
        || is_crew_dead;
}

inline bool UnitCanFire(const WeaponSystemInGroup& weapon)
{
    return !UnitDestroyed(weapon) &&
        HAS_NO_FLAG(weapon.statuses, WeaponSystemStatus::TurretJammed) &&
        HAS_NO_FLAG(weapon.statuses, WeaponSystemStatus::GunnerKilled);
}

inline bool UnitDamaged(const WeaponSystemInGroup& weapon)
{
    return HAS_FLAG(weapon.statuses, WeaponSystemStatus::Immobilized)
        || HAS_FLAG(weapon.statuses, WeaponSystemStatus::TurretJammed)
        || HAS_FLAG(weapon.statuses, WeaponSystemStatus::DriverKilled)
        || HAS_FLAG(weapon.statuses, WeaponSystemStatus::GunnerKilled)
        || HAS_FLAG(weapon.statuses, WeaponSystemStatus::CmdrKilled);
}
BattleGroup FormBattleGroup(Armory* armory, u32 parent_unit_index, Unit& unit);

std::vector<f32> GetModifiers(SimulationParams& params, WeaponSystemGeneralType type, SideStatus status);
bool MoralBroke(std::vector<BattleGroup>& groups, f32 threshold);
bool AverageDamageExceedsThreshold(std::vector<BattleGroup>& groups, f32 threshold);
PriorityQueue ConstructPriorityQueue(Armory*, const std::vector<BattleGroup>&);
TargetingInfo TryTargeting(Armory*, const WeaponSystemInGroup& firing_weapon, std::vector<BattleGroup>& enemy_groups, PriorityQueue, u32 distance);
std::tuple<bool, f32> TryToHitTarget(TargetingInfo&, TryToHitParams&, u32 distance);

std::vector<FireResult> Fire(
    Side firing_side,
    Armory* armory,
    SimulationParams&,
    u16 distance_in_m,
    const std::vector<BattleGroup>& attacking_battlegroups,
    std::vector<BattleGroup>& targeted_battlegroups);

#endif
