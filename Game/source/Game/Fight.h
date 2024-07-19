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

#include "Entities.h"
#include "Modifiers.h"

class WeatherManager;
// class Modifier;
// class ModifiersManager;
enum class HitDirection {
    Top,

    HullFront,
    HullSide,
    HullRear,

    TurretFront,
    TurretSide,
    TurretRear,
};

enum class UnitSize {
    Division,
    Brigade,
    Regiment,
    Battalion,
    Company,
    Platoon,
    Squad,
};

enum class DamageType {
    Kinetic,
    HEAT,
    Tandem,
    HE
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

const BiMap<std::string, DamageType> STR_TO_DAMAGE_TYPE = {
    { "HE", DamageType::HE },
    { "HEAT", DamageType::HEAT },
    { "Kinetic", DamageType::Kinetic },
    { "Tandem", DamageType::Tandem },
};

enum class WeaponDomain {
    Cyber,
    Ground,
    Air,
    Sea,
};

enum class WeaponSystemGeneralType {
    Infantry,
    APC,
    IFV,
    Artillery,
    Mortar,
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
    std::string id;
    std::string name;
    WeaponDomain domain; // Domain of the potential target
    DamageType damage_type;
    u16 penetration;
    u16 fragmentation;
    std::vector<Accuracy> accuracy;
};

struct Armor {
    u32 top;

    u32 hull_front;
    u32 hull_rear;
    u32 hull_side;

    u32 turret_front;
    u32 turret_rear;
    u32 turret_side;
};

struct Device {
    std::string id;
    std::string name;
    std::set<u32> ammunition;
};

class Armory;
class WeaponSystem {
public:
    std::string id;
    Armor armor;
    WeaponDomain domain;
    WeaponSystemGeneralType type;

    std::string name;

    u16 image_pos_on_atlas = 0;
    u32 cost_in_dollars = 0;

    std::set<u32> devices;

    std::map<WeaponDomain, u32> cached_max_range_per_domain_soft;

    u32 GetArmorAt(HitDirection);
    bool IsArmored();
};

struct Capabilities {
    bool hard;
    bool soft;

    std::set<WeaponDomain> domains;
};
Capabilities GetCapabilities(Armory*, Device&);

enum class Side {
    UA,
    RU
};

struct Commander {
    char* name;
};

struct Unit {
    std::string id;
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

    WeaponSystem* GetWeaponById(std::string_view id);
    std::optional<u32> GetWeaponIndexByName(std::string_view name);
    std::optional<u32> GetDeviceIndexById(std::string_view device_id);
    std::optional<u32> GetAmmoIndexById(std::string_view ammo_id);

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

using UnitId    = std::string;
using UnitIndex = u32;

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

    std::string ToString() const
    {
        std::stringstream ss;

        ss << run << ";";
        ss << round << ";";
        ss << distance << "";

        return ss.str();
    }
};

std::string DomainToStr(WeaponDomain domain);

std::string WeaponTypeToStr(WeaponSystemGeneralType type);

struct WeaponInGroup {
    WeaponSystem* weapon;
    f32 morale;
    f32 state;
    f32 initial_state;
};

constexpr u32 MAX_BG_SIZE = 128;
struct BattleGroup {
    std::string name;
    std::string Side;

    std::array<WeaponInGroup, MAX_BG_SIZE> weapons;

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
            const auto state = w.state;
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
                ss << state << "\n";
            }
            index++;
        }
    }
};

enum class SideStatus {
    Attacking,
    Defending,
};

enum class UnitStance {
    None,
    Committed,
    Defending,
    Reserve,
    Redeploying,
    Retreating,
    Routing,
    Resting,
};

constexpr u8 MAX_UNITS = 24;
constexpr u8 SUPPORT_ASSETS = 8;

WeaponSystemGeneralType StrToWeaponType(std::string& str);

struct Loss {
    u32 index_in_armory;
    u32 number_lost;
    f32 percent_lost_from_bg;
};

struct AttackResult {
    SideStatus winner_status;
    Side winner_side;

    std::vector<Loss> attacker_weapon_index_by_numbers_lost;
    std::vector<Loss> defender_weapon_index_by_numbers_lost;
};

struct TargetingInfo {
    bool can_fire;
    BattleGroup* targeted_group;
    WeaponInGroup* targeted_weapon;
    WeaponInGroup* weapon_to_use;
    Device* device_to_use;
    Ammo* ammo_to_use;
};

struct FireResult {
    u64 run;
    u64 round;
    std::string firing_side;
    std::string status;
    TargetingInfo targeting_info;
    f32 acc;
    f32 starting_state;
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

std::vector<f32> GetModifiers(SimulationParams& params, WeaponSystemGeneralType type, SideStatus status);
bool MoralBroke(std::vector<BattleGroup>& groups, f32 threshold);
bool AverageDamageExceedsThreshold(std::vector<BattleGroup>& groups, f32 threshold);
TargetingInfo TryPickingTarget(const WeaponInGroup& firing_weapon, std::mt19937& mt, std::vector<BattleGroup>& enemy_groups, u32 distance);
std::tuple<bool, f32> TryToHitTarget(TargetingInfo&, std::mt19937&, u32 distance);

bool IsAP(Ammo*);

bool IsHE(Ammo*);

std::vector<FireResult> Fire(
    Side firing_side,
    Armory* armory,
    SimulationParams&,
    u16 distance_in_m,
    const std::vector<BattleGroup>& attacking_battlegroups,
    std::vector<BattleGroup>& targeted_battlegroups);

#endif
