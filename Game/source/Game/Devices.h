#pragma once

#include <optional>
#include <vector>
#include "Fight.h"

std::optional<u32> GetCommanderIndexByName(const std::string& name);
std::optional<u32> GetWeaponIndexByName(std::vector<WeaponSystem>& weapons, const std::string& name);
void AddUnitToState(Unit& unit, const std::string& oblast_name);

void PrintArmory(Armory& armory);
Armory LoadArmory(const char* path, const char* storage_path);
Deployment LoadUnits(std::vector<WeaponSystem>& weapons, BiMap<OblastCode, const std::string>& oblast_names, const char* path);

WeaponSystemGeneralType StrToWeaponType(std::string& str);
std::string WeaponTypeToStr(WeaponSystemGeneralType type);

//----
std::vector<std::string> split(const std::string& s, char delimiter);
Armor StrToArmor(std::string& str);
WeaponDomain DomainStrToEnum(std::string& text);
std::vector<Accuracy> StrToAccuracy(std::string& text);
static void remove(std::string& str, char c);
static void fill(std::vector<u32>& v, u32 value);
Side StrToSide(const std::string& str);
std::string SideToStr(Side side);
