#include "Devices.h"

#include <fstream>
#include <map>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

constexpr char WEAPON_CHAR = '<';
constexpr char DEVICE_CHAR = ':';

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

WeaponDomain DomainStrToEnum(std::string& text)
{
    if (text == "Ground")
        return WeaponDomain::Ground;
    if (text == "Air")
        return WeaponDomain::Air;
    if (text == "Sea")
        return WeaponDomain::Sea;
    if (text == "Cyber")
        return WeaponDomain::Cyber;

    assert(false);

    return WeaponDomain::Cyber;
}

std::vector<Accuracy> StrToAccuracy(std::string& text)
{
    std::vector<Accuracy> ratings;
    auto parts = split(text, ',');
    for (u32 i = 0; i < parts.size(); i += 2) {
        u32 range = std::stoi(parts[i]);
        f32 acc = std::stof(parts[i + 1]);
        ratings.push_back({ range, acc });
    }
    return ratings;
}

std::optional<u32> Armory::GetWeaponIndexByName(const std::string& name)
{
    u32 i = 0;
    for (auto& w : weapons) {
        if (w.name == name)
            return { i };
    }
    return std::nullopt;
}

void PrintArmory(Armory& armory)
{
    u32 index = 0;
    for (auto& w : armory.weapons) {
        printf("%s UA: %i RU: %i\n", w.name.c_str(), armory.ua_weapons_quantity[index], armory.ru_weapons_quantity[index]);
        for (u32 d : w.devices) {
            printf("\t%s\n", armory.devices[d].name.c_str());

            for (u32 a : armory.devices[d].ammunition) {
                auto& ammo = armory.ammo[a];
                printf("\t\tAmmo: %s Damage Type: %s Damage: %i UA Qty: %i RU Qty: %i\n", ammo.name.c_str(), STR_TO_DAMAGE_TYPE.GetKey(ammo.damage_type), ammo.penetration, armory.ua_ammo_quantity[a], armory.ru_ammo_quantity[a]);
                for (auto& acc : ammo.accuracy) {
                    printf("\t\t\t %f at %im\n", acc.change_to_hit, acc.range_in_meters);
                }
                printf("\n");
            }
        }
        index++;
    }
}

static void remove(std::string& str, char c)
{
    str.erase(std::remove(str.begin(), str.end(), c), str.end());
}

static void fill(std::vector<u32>& v, u32 value)
{
    for (u32 i = 0; i < v.capacity(); i++) {
        v.push_back(value);
    }
}

WeaponSystemGeneralType StrToWeaponType(std::string& str)
{
    return WEAPON_TYPE_STRING_MAP.GetKey(str);
}

std::string WeaponTypeToStr(WeaponSystemGeneralType type)
{
    return WEAPON_TYPE_STRING_MAP.GetValue(type);
}

Armory LoadArmory(const char* weapons_path, const char* storage_path)
{
    Armory armory;
    std::vector<WeaponSystem> weapons;
    std::vector<Device> devices;
    std::vector<Ammo> ammunition;

    std::ifstream file(weapons_path);

    if (!file.is_open()) {
        printf("Failed to read data file\n");
        assert(false);
    }

    WeaponSystem* weapon_system = nullptr;
    Device* device = nullptr;

    std::string line;

    std::map<std::string, u32> device_name_by_index;
    std::map<std::string, u32> weapon_name_by_index;
    std::map<std::string, u32> ammo_name_by_index;

    getline(file, line);

    while (getline(file, line)) {
        if (line == "" || line.starts_with('#'))
            continue;

        auto parts = split(line, ';');

        if (parts[0] == "")
            continue;

        if (line.starts_with(WEAPON_CHAR)) {
            WeaponSystem w;
            w.name = parts[0].substr(1);
            w.default_state = std::stoi(parts[1]);
            w.domain = DomainStrToEnum(parts[4]);
            w.type = StrToWeaponType(parts[6]);

            weapons.push_back(w);
            weapon_system = &weapons[weapons.size() - 1];
            weapon_name_by_index.insert(std::pair(parts[0], weapons.size() - 1));
        } else if (line.starts_with(DEVICE_CHAR)) {
            if (device_name_by_index.contains(parts[0])) {
                weapon_system->devices.push_back(device_name_by_index.at(parts[0]));
                device = &devices[device_name_by_index.at(parts[0])];
            } else {
                Device d;
                d.name = parts[0].substr(1);
                devices.push_back(d);
                device = &devices[devices.size() - 1];

                weapon_system->devices.push_back(devices.size() - 1);
                device_name_by_index.insert(std::pair(device->name, devices.size() - 1));
            }
        } else {
            Ammo ammo;
            ammo.name = parts[0];
            ammo.domain = DomainStrToEnum(parts[1]);
            ammo.damage_type = STR_TO_DAMAGE_TYPE.GetValue(parts[2]);
            ammo.penetration = std::stoi(parts[3]);
            ammo.accuracy = StrToAccuracy(parts[4]);

            ammunition.push_back(ammo);
            device->ammunition.insert(ammunition.size() - 1);
            ammo_name_by_index.insert(std::pair(parts[0], ammunition.size() - 1));
        }
    }

    armory.weapons = weapons;
    armory.devices = devices;
    armory.ammo = ammunition;

    assert(armory.weapons.size() > 0);
    assert(armory.devices.size() > 0);
    assert(armory.ammo.size() > 0);

    file.close();

    // Load quantities
    std::ifstream storage_file(storage_path);
    if (!storage_file.is_open()) {
        printf("Failed to read storage file\n");
        assert(false);
    }
    armory.ua_weapons_quantity.reserve(armory.weapons.size());
    armory.ua_ammo_quantity.reserve(armory.ammo.size());

    armory.ru_weapons_quantity.reserve(armory.weapons.size());
    armory.ru_ammo_quantity.reserve(armory.ammo.size());

    fill(armory.ua_weapons_quantity, 0);
    fill(armory.ua_ammo_quantity, 0);

    fill(armory.ru_weapons_quantity, 0);
    fill(armory.ru_ammo_quantity, 0);

    getline(storage_file, line);
    // NOTE: slow af
    while (getline(storage_file, line)) {
        auto parts = split(line, ';');

        std::string& asset_name = parts[0];
        std::string ua_quantity_str = parts[1];
        std::string ru_quantity_str = parts[2];

        u64 ua_quantity = std::stoi(ua_quantity_str);
        u64 ru_quantity = std::stoi(ru_quantity_str);

        if (asset_name.starts_with(WEAPON_CHAR)) {
            assert(weapon_name_by_index.contains(asset_name));
            u32 weapon_index = weapon_name_by_index.at(asset_name);
            armory.ua_weapons_quantity[weapon_index] = ua_quantity;
            armory.ru_weapons_quantity[weapon_index] = ru_quantity;
        } else {
            assert(ammo_name_by_index.contains(asset_name));
            u32 ammo_index = ammo_name_by_index.at(asset_name);
            armory.ua_ammo_quantity[ammo_index] = ua_quantity;
            armory.ru_ammo_quantity[ammo_index] = ru_quantity;
        }
    }

    return armory;
}

std::string SideToStr(Side side)
{
    if (side == Side::UA)
        return "UA";

    if (side == Side::RU)
        return "RU";

    assert(false);
}

Side StrToSide(const std::string& str)
{
    assert(str == "UA" || str == "RU");

    if (str == "UA")
        return Side::UA;
    if (str == "RU")
        return Side::RU;
}

std::optional<u32> GetCommanderIndexByName(const std::string& name)
{
    u32 index = 0;
    for (Commander& cmdr : STATE.commanders) {
        if (cmdr.name == name)
            return { index };
        index++;
    }
    return std::nullopt;
}

std::optional<u32> GetWeaponIndexByName(std::vector<WeaponSystem>& weapons, const std::string& name)
{
    u32 index = 0;
    for (WeaponSystem& w : weapons) {
        if (w.name == name) {
            return { index };
        }
        index++;
    }
    return std::nullopt;
}

static void AddUnitToState(Unit& unit, const std::string& oblast_name)
{
    if (unit.side == Side::UA) {
        assert(OBLAST_NAMES.ContainsValue(oblast_name));
        STATE.troops_deployment.ukr_units.push_back(unit);
        STATE.troops_deployment.ukr_assigned.push_back(OBLAST_NAMES.GetKey(oblast_name));
    } else {
        assert(OBLAST_NAMES.ContainsValue(oblast_name));
        STATE.troops_deployment.ru_units.push_back(unit);
        STATE.troops_deployment.ru_assigned.push_back(OBLAST_NAMES.GetKey(oblast_name));
    }
}

Deployment LoadUnits(std::vector<WeaponSystem>& weapons, const BiMap<OblastCode, const std::string>& oblast_names, const char* path)
{
    Deployment deployment;

    std::ifstream storage_file(path);
    if (!storage_file.is_open()) {
        printf("Failed to read units file\n");
        assert(false);
    }

    std::string line;
    getline(storage_file, line);

    Unit unit;
    bool seen_unit = false;
    std::vector<std::string> parts;
    std::string last_oblast;

    while (getline(storage_file, line)) {
        if (line == "" || line.starts_with('#'))
            continue;

        parts = split(line, ';');

        if (parts[0].starts_with('<')) {
            if (seen_unit) {
                if (unit.side == Side::RU) {
                    deployment.ru_units.push_back(unit);
                    deployment.ru_assigned.push_back(oblast_names.GetKey(last_oblast));
                } else {
                    deployment.ukr_units.push_back(unit);
                    deployment.ukr_assigned.push_back(oblast_names.GetKey(last_oblast));
                }
                unit = Unit();
            }

            unit.name = parts[0].substr(1);
            unit.nickname = parts[1];
            unit.side = StrToSide(parts[2]);
            last_oblast = parts[4];
            unit.size = STR_TO_SIZE.at(parts[5]);
            seen_unit = true;
            /*
            auto cmdr = GetCommanderIndexByName(parts[3]);
            assert(cmdr.has_value());
            unit.commander_index = *cmdr;
            */

        } else {
            auto weapon_index = GetWeaponIndexByName(weapons, parts[0]);
            assert(weapon_index.has_value());
            unit.weapons.push_back(*weapon_index);
            unit.weapons_toe.push_back(std::stoi(parts[1]));
            unit.weapons_counter.push_back(std::stoi(parts[2]));
            unit.morale.push_back(std::stof(parts[3]));
        }
    }

#if DEBUG
    PrintUnit(unit);
#endif

    if (unit.side == Side::RU) {
        deployment.ru_units.push_back(unit);
        deployment.ru_assigned.push_back(oblast_names.GetKey(last_oblast));
    } else {
        deployment.ukr_units.push_back(unit);
        deployment.ukr_assigned.push_back(oblast_names.GetKey(last_oblast));
    }

    return deployment;
}
