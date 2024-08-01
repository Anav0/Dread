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

struct Unit {
    std::string id;
    std::string name;
    std::string nickname;
    UnitSize size;
    Side side;
    UnitStance stance;
    UnitType type;
    u32 commander_index;

    std::vector<u32> weapons;
    std::vector<f32> morale;
    std::vector<u16> weapons_counter;
    std::vector<u16> weapons_toe;
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

static const BiMap<std::string, UnitType> STR_TO_UNIT_TYPE = {
    { "Panzer", UnitType::Panzer },
    { "Infantry", UnitType::Infantry },
    { "Mech", UnitType::Mech },
    { "Airborn", UnitType::Airborn },
    { "Static", UnitType::Static },
};

static const std::map<std::string, UnitSize> STR_TO_SIZE = {
    { "Division", UnitSize::Division },
    { "Brigade", UnitSize::Brigade },
    { "Regiment", UnitSize::Regiment },
    { "Battalion", UnitSize::Battalion },
    { "Company", UnitSize::Company },
    { "Platoon", UnitSize::Platoon },
    { "Squad", UnitSize::Squad },
};

static const BiMap<std::string, UnitStance> STR_TO_UNIT_STANCE = {
    { "Commited", UnitStance::Commited },
    { "Defending", UnitStance::Defending },
    { "None", UnitStance::None },
    { "Redeploying", UnitStance::Redeploying },
    { "Reserve", UnitStance::Reserve },
    { "Resting", UnitStance::Resting },
    { "Routing", UnitStance::Routing },
};

static const BiMap<std::string, DamageType> STR_TO_DAMAGE_TYPE = {
    { "HE", DamageType::HE },
    { "HEAT", DamageType::HEAT },
    { "Kinetic", DamageType::Kinetic },
    { "Tandem", DamageType::Tandem },
};

static const BiMap<WeaponSystemStatus, std::string> STR_TO_UNIT_STATUS = {
    { WeaponSystemStatus::OnFire, "OnFire" },
    { WeaponSystemStatus::Immobilized, "Immobilized" },
    { WeaponSystemStatus::DriverKilled, "DriverKilled" },
    { WeaponSystemStatus::GunnerKilled, "GunnerKilled" },
    { WeaponSystemStatus::CmdrKilled, "CmdrKilled" },
    { WeaponSystemStatus::Abondoned, "Abondoned" },
    { WeaponSystemStatus::TurretJammed, "TurretJammed" },
};

static const BiMap<WeaponSystemGeneralType, std::string> WEAPON_TYPE_STRING_MAP = {
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
