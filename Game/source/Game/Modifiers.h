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
class WeaponSystemInGroup;
class TryToHitParams;

using ModifierFn = std::function<void(Armory*, const WeaponSystemInGroup&, SimulationParams&)>;

class Modifier {
public:
    Side side;
    std::string name;
    ModifierFn fn;

    Modifier() = default;
    Modifier(Side side, std::string name, ModifierFn fn) : side(side), name(name), fn(fn) { }
};

class ModifiersManager {
public:
    std::vector<Modifier> modifiers;

    void AddModifier(Side side, std::string name, ModifierFn fn);
};

class TargetingInfo;

//TODO: move from here
bool ArmorWasPenetrated(TargetingInfo&);

//NOTE: To be implemented by Game.cpp (Game) or main.cpp (Analysis)
void AddModifiers(ModifiersManager& manager);

#endif
