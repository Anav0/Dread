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
    for (auto& w : armory.weapons) {
        printf("%s\n", w.name.c_str());
        for (u32 d : w.devices) {
            printf("\t%s\n", armory.devices[d].name.c_str());

            for (u32 a : armory.devices[d].ammunition) {
                auto& ammo = armory.ammo[a];
                printf("\t\tAmmo: %s Soft: %i Hard: %i\n", ammo.name.c_str(), ammo.soft, ammo.hard);
                for (auto& acc : ammo.accuracy) {
                    printf("\t\t\t %f at %im\n", acc.change_to_hit, acc.range_in_meters);
                }
                printf("\n");
            }
        }
    }
}

Armory LoadArmory(const char* path)
{
    Armory armory;
    std::vector<WeaponSystem> weapons;
    std::vector<Device> devices;
    std::vector<Ammo> ammunition;

    std::ifstream file("D:/Projects/Dread/Game/data/data.info");

    if (!file.is_open()) {
        printf("Failed to read data file\n");
        assert(false);
    }

    WeaponSystem* weapon_system = nullptr;
    Device* device = nullptr;

    std::string line;

    std::map<std::string, u32> device_name_by_index;

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
        }
    }

    armory.weapons = weapons;
    armory.devices = devices;
    armory.ammo = ammunition;

    file.close();

    return armory;
}
