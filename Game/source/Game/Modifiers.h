#ifndef MODIFIER_H
#define MODIFIER_H

#include <functional>
#pragma once

#include "Misc/Constants.h"

#include <memory>
#include <string>
#include <array>
#include <vector>
#include <map>

enum class WeaponSystemGeneralType;
enum class SideStatus;
enum class Side;
enum class Weather;
enum class GroundCondition;
enum class OblastCode;

class SimulationParams;
class WeaponSystem;
class Armory;
class WeatherManager;

using ConditionFn = std::function<bool(Armory*, WeaponSystem& weapon_used)>;

class Modifier {
private:

public:
    Side side;
    std::string name;
    ConditionFn condition;

    Modifier() = default;
    Modifier(Side side, std::string name, ConditionFn condition) : side(side), name(name), condition(condition) { }
};

class ModifiersManager {
public:
    std::vector<Modifier> modifiers;

    void AddModifier(Side side, std::string name, ConditionFn fn);
    void Test();
};

class TargetingInfo;

//TODO: move from here
bool ArmorWasPenetrated(TargetingInfo&);

#endif
