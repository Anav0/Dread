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

void PrintArmory(Armory& armory)
{
    u32 index = 0;
    for (auto& w : armory.weapons) {
        printf("%s UA: %i RU: %i\n", w.name.c_str(), armory.ua_weapons_quantity[index], armory.ru_weapons_quantity[index]);
        for (u32 d : w.devices) {
            printf("\t%s\n", armory.devices[d].name.c_str());

            for (u32 a : armory.devices[d].ammunition) {
                auto& ammo = armory.ammo[a];
                printf("\t\tAmmo: %s Soft: %i Hard: %i UA: %i RU: %i\n", ammo.name.c_str(), ammo.soft, ammo.hard, armory.ua_ammo_quantity[a], armory.ru_ammo_quantity[a]);
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

static void fill(std::vector<u32>& v, u32 value) {
    for (u32 i = 0; i < v.capacity(); i++) {
        v.push_back(value);
    }
}

Armory LoadArmory(const char* path, const char* storage_path)
{
    Armory armory;
    std::vector<WeaponSystem> weapons;
    std::vector<Device> devices;
    std::vector<Ammo> ammunition;

    std::ifstream file(path);

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

    while (getline(file, line)) {
        if (line == "" || line.starts_with('#'))
            continue;

        auto parts = split(line, ' ');

        if (line.starts_with(WEAPON_CHAR)) {
            WeaponSystem w;
            w.name = parts[0].substr(1);
            w.state = std::stoi(parts[1]);
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
            ammo.soft = std::stoi(parts[2]);
            ammo.hard = std::stoi(parts[3]);
            ammo.AA = std::stoi(parts[4]);
            ammo.accuracy = StrToAccuracy(parts[5]);

            ammunition.push_back(ammo);
            device->ammunition.insert(ammunition.size() - 1);
            ammo_name_by_index.insert(std::pair(parts[0], ammunition.size() - 1));
        }
    }

    armory.weapons = weapons;
    armory.devices = devices;
    armory.ammo = ammunition;

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
    //NOTE: slow af
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
