#pragma once

#include <optional>
#include <vector>
#include <string>
#include <string_view>

#include "Fight.h"
#include "Units.h"

constexpr char USE_NAME_AS_ID_CHAR = '-';
constexpr char COMMON_SPLIT_CHAR   = ';';

std::optional<u32> GetCommanderIndexByName(const std::string& name);
std::optional<u32> GetWeaponIndexByName(std::vector<WeaponSystem>& weapons, const std::string& name);
void AddUnitToState(Unit& unit, const std::string& oblast_name);

void PrintArmory(Armory& armory);

Armory LoadArmory(const char* weapons_path, const char* ammo_path, const char* devices_path, const char* storage_path);
void LoadAmmo(Armory* armory, const char* path);
void LoadDevices(Armory* armory, const char* path);
Deployment LoadDeployment(Armory* armory, const std::string& path);

std::set<u32> ParseDevices(Armory* armory, std::string& devices_str);
std::set<u32> ParseAmmo(Armory* armory, std::string& ammo_str);

//----
std::vector<std::string> split(const std::string& s, char delimiter);
std::vector<Accuracy> StrToAccuracy(std::string& text);
static void remove(std::string& str, char c);
static void fill(std::vector<u32>& v, u32 value);
Side StrToSide(const std::string& str);
std::string SideToStr(Side side);

