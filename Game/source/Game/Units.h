#ifndef UNITS_H
#define UNITS_H

#pragma once

#include <array>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "Misc/Flags.h"
#include "Ethnicity.h"

class Armory;

using UnitId = std::string;
using UnitIndex = u32;

enum class HitDirection {
    Top,

    HullFront,
    HullSide,
    HullRear,

    TurretFront,
    TurretSide,
    TurretRear,
};

enum class UnitStance {
    None,
    Commited,
    Defending,
    Reserve,
    Redeploying,
    Retreating,
    Routing,
    Resting,
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

enum class UnitType {
    Panzer,
    Infantry,
    Mech,
    Airborn,
    Static,
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

class WeaponSystem {
public:
    std::string id;
    Armor armor;
    WeaponDomain domain;
    WeaponSystemGeneralType type;

    std::string name;

    u16 image_pos_on_atlas = 0;
    u32 cost_in_dollars    = 0;

    // Computed in Init()
    u32 max_distance      = 0;
    u32 max_penetration   = 0;
    u32 max_fragmentation = 0;

    std::set<u32> devices;

    void Precompute(Armory*);

    bool CanBePenetrated(Ammo*, HitDirection) const;
    bool CanPenetrate(WeaponSystem*, HitDirection) const;
    std::pair<Device*, Ammo*> PickRightDevice(Armory*, const WeaponSystem*, u32 distance) const;
    constexpr u32 GetArmorAt(HitDirection);
    bool CanReach(u32 distance_m) const;
    bool IsArmored() const;
    bool IsArtillery() const;
    bool IsAA() const;
};

enum class Side {
    UA,
    RU
};

struct Commander {
    char* name;
};

struct WeaponInfo {
    u32 index_in_armory;
    f32 morale;
    u16 initial_n;
    u16 n;

WeaponInfo(const u32& index_in_armory, const u16& initial_n, const u16& n, const f32& morale)
        : index_in_armory(index_in_armory)
        , morale(morale)
        , initial_n(initial_n)
        , n(n)
    {
    }
};

struct EthnicityInfo {
    Ethnicity ethnicity;
    f32 percent;
};

struct Unit {
    std::string id;
    std::string name;
    std::string nickname;
    UnitSize size;
    Side side;
    UnitStance stance;
    UnitType type;
    u32 commander_index;

    std::vector<WeaponInfo> weapons;
    std::vector<EthnicityInfo> ethnic_info;
};

enum WeaponSystemStatus : u32 {
    OnFire       = FLAG(0),
    Immobilized  = FLAG(1),
    DriverKilled = FLAG(2),
    GunnerKilled = FLAG(3),
    CmdrKilled   = FLAG(4),
    Abondoned    = FLAG(5),
    TurretJammed = FLAG(6),
};

static const BiMap<UnitType, std::string> UNIT_TYPE_MAP = {
    { UnitType::Panzer, "Panzer" },
    { UnitType::Infantry, "Infantry" },
    { UnitType::Mech, "Mech" },
    { UnitType::Airborn, "Airborn" },
    { UnitType::Static, "Static" },
};

static const BiMap<UnitSize, std::string> UNIT_SIZE_MAP = {
    { UnitSize::Division, "Division" },
    { UnitSize::Brigade, "Brigade" },
    { UnitSize::Regiment, "Regiment" },
    { UnitSize::Battalion, "Battalion" },
    { UnitSize::Company, "Company" },
    { UnitSize::Platoon, "Platoon" },
    { UnitSize::Squad, "Squad" },
};

static const BiMap<UnitStance, std::string> UNIT_STANCE_MAP = {
    { UnitStance::Commited, "Commited" },
    { UnitStance::Defending, "Defending" },
    { UnitStance::None, "None" },
    { UnitStance::Redeploying, "Redeploying" },
    { UnitStance::Reserve, "Reserve" },
    { UnitStance::Resting, "Resting" },
    { UnitStance::Routing, "Routing" },
};

static const BiMap<DamageType, std::string> DAMAGE_TYPE_MAP = {
    { DamageType::HE, "HE" },
    { DamageType::HEAT, "HEAT" },
    { DamageType::Kinetic, "Kinetic" },
    { DamageType::Tandem, "Tandem" },
};

static const BiMap<WeaponSystemStatus, std::string> WEAPON_SYSTEM_STATUS_MAP = {
    { WeaponSystemStatus::OnFire, "OnFire" },
    { WeaponSystemStatus::Immobilized, "Immobilized" },
    { WeaponSystemStatus::DriverKilled, "DriverKilled" },
    { WeaponSystemStatus::GunnerKilled, "GunnerKilled" },
    { WeaponSystemStatus::CmdrKilled, "CmdrKilled" },
    { WeaponSystemStatus::Abondoned, "Abondoned" },
    { WeaponSystemStatus::TurretJammed, "TurretJammed" },
};

static const BiMap<WeaponSystemGeneralType, std::string> WEAPON_SYSTEM_GENERAL_TYPE_MAP = {
    { WeaponSystemGeneralType::Drone, "Drone" },
    { WeaponSystemGeneralType::Infantry, "Infantry" },
    { WeaponSystemGeneralType::APC, "APC" },
    { WeaponSystemGeneralType::Artillery, "Artillery" },
    { WeaponSystemGeneralType::Vehicle, "Vehicle" },
    { WeaponSystemGeneralType::IFV, "IFV" },
    { WeaponSystemGeneralType::Tank, "Tank" },
    { WeaponSystemGeneralType::MLRS, "MLRS" },
    { WeaponSystemGeneralType::MANPADS, "MANPADS" },
    { WeaponSystemGeneralType::ATGM, "ATGM" },
    { WeaponSystemGeneralType::Mortar, "Mortar" },
};

static const BiMap<WeaponDomain, std::string> WEAPON_DOMAIN_MAP = {
    { WeaponDomain::Ground, "Ground" },
    { WeaponDomain::Air, "Air" },
    { WeaponDomain::Sea, "Sea" },
    { WeaponDomain::Cyber, "Cyber" },
};

inline std::string StatusesToStr(u32 statuses)
{
    return "TMP";
}

WeaponSystemGeneralType StrToWeaponType(std::string& str);
std::string WeaponTypeToStr(WeaponSystemGeneralType type);

std::string DomainToStr(WeaponDomain domain);

void PrintUnit(Armory& armory, Unit& unit);
bool IsAP(Ammo*);
bool IsHE(Ammo*);

#endif
