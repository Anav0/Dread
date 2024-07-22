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

std::optional<u32> Armory::GetAmmoIndexById(std::string_view ammo_id)
{
    u32 i = 0;

    for (auto& a : ammo) {
        if (a.id == ammo_id) {
            return { i };
        }
        i++;
    }
    return std::nullopt;
}

std::optional<u32> Armory::GetDeviceIndexById(std::string_view device_id)
{
    u32 i = 0;
    for (auto& d : devices) {
        if (d.id == device_id)
            return { i };
        i++;
    }
    return std::nullopt;
}

// Speed:
// Pointer:
std::optional<u32> Armory::GetWeaponIndexById(std::string_view id)
{
    u32 i = 0;
    for (auto& w : weapons) {
        if (w.id == id)
            return { i };
        i++;
    }

    return std::nullopt;
}


// Speed:
// Pointer:
WeaponSystem* Armory::GetWeaponById(std::string_view id)
{
    for (auto& w : weapons) {
        if (w.id == id)
            return &w;
    }
}

std::optional<u32> Armory::GetWeaponIndexByName(std::string_view name)
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

std::set<u32> ParseAmmo(Armory* armory, std::string& ammo_str)
{
    auto ids = split(ammo_str, ',');

    std::set<u32> ammo_ids;
    for (std::string_view id : ids) {
        auto ammo_id = armory->GetAmmoIndexById(id);
        assert(ammo_id.has_value());
        ammo_ids.insert(ammo_id.value());
    }

    return ammo_ids;
}
std::set<u32> ParseDevices(Armory* armory, std::string& devices_str)
{
    auto ids = split(devices_str, ',');

    std::set<u32> device_ids;
    for (const auto& id : ids) {
        auto device_id = armory->GetDeviceIndexById(id);
        assert(device_id.has_value());
        device_ids.insert(device_id.value());
    }

    return device_ids;
}

void LoadAmmo(Armory* armory, const char* path)
{
    std::ifstream file(path);

    if (!file.is_open()) {
        printf("Failed to read weapons file\n");
        assert(false);
    }

    std::string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line == "")
            continue;

        auto parts = split(line, COMMON_SPLIT_CHAR);

        Ammo a;
        a.id = parts[0][0] == USE_NAME_AS_ID_CHAR ? parts[1] : parts[0];
        a.name = parts[1];
        a.domain = DomainStrToEnum(parts[2]);
        a.damage_type = STR_TO_DAMAGE_TYPE.GetValue(parts[3]);
        a.penetration = std::stoi(parts[4]);
        a.fragmentation = std::stoi(parts[5]);
        a.accuracy = StrToAccuracy(parts[6]);

        armory->ammo.push_back(a);
    }

    file.close();
}

void LoadDevices(Armory* armory, const char* path)
{
    std::ifstream file(path);

    if (!file.is_open()) {
        printf("Failed to read weapons file\n");
        assert(false);
    }

    std::string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line == "")
            continue;

        auto parts = split(line, COMMON_SPLIT_CHAR);

        Device d;
        d.id = parts[0][0] == USE_NAME_AS_ID_CHAR ? parts[1] : parts[0];
        d.name = parts[1];
        d.ammunition = ParseAmmo(armory, parts[2]);

        armory->devices.push_back(d);
    }

    file.close();
}

void LoadWeapons(Armory* armory, const char* path)
{
    std::ifstream file(path);

    if (!file.is_open()) {
        printf("Failed to read weapons file\n");
        assert(false);
    }

    std::string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line == "")
            continue;

        auto parts = split(line, COMMON_SPLIT_CHAR);

        WeaponSystem w;
        w.id = parts[0][0] == USE_NAME_AS_ID_CHAR ? parts[1] : parts[0];
        w.name = parts[1];

        w.armor.top = std::stoi(parts[2]);

        w.armor.hull_front = std::stoi(parts[3]);
        w.armor.hull_rear = std::stoi(parts[4]);
        w.armor.hull_side = std::stoi(parts[5]);

        w.armor.turret_front = std::stoi(parts[6]);
        w.armor.turret_rear = std::stoi(parts[7]);
        w.armor.turret_side = std::stoi(parts[8]);

        w.domain = DomainStrToEnum(parts[23]);
        w.type = StrToWeaponType(parts[24]);
        // NOTE: We ignore image id
        w.cost_in_dollars = std::stoi(parts[26]);

        // SPEED:
        w.devices = ParseDevices(armory, parts[27]);

        armory->weapons.push_back(w);
    }

    file.close();
}

// TODO: for now we assume both sides have plenty of stuff
void LoadQuantities(Armory* armory, const char* path)
{
    // std::ifstream storage_file(path);
    // if (!storage_file.is_open()) {
    //     printf("Failed to read storage file\n");
    //     assert(false);
    // }
    armory->ua_weapons_quantity.reserve(armory->weapons.size());
    armory->ua_ammo_quantity.reserve(armory->ammo.size());

    armory->ru_weapons_quantity.reserve(armory->weapons.size());
    armory->ru_ammo_quantity.reserve(armory->ammo.size());

    fill(armory->ua_weapons_quantity, 10000);
    fill(armory->ua_ammo_quantity, 10000);

    fill(armory->ru_weapons_quantity, 10000);
    fill(armory->ru_ammo_quantity, 10000);
}

Armory LoadArmory(const char* weapons_path, const char* ammo_path, const char* devices_path, const char* storage_path)
{
    Armory armory;

    // NOTE: order matters!
    LoadAmmo(&armory, ammo_path);
    LoadDevices(&armory, devices_path);
    LoadWeapons(&armory, weapons_path);
    LoadQuantities(&armory, storage_path);

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
    assert(OBLASTS.ContainsValue(oblast_name));
    STATE.troops_deployment.Insert(unit, OBLASTS.GetKey(oblast_name));
}

void LoadUnitOOB(Armory* armory, Deployment* deployment, const std::string& oob_path)
{
    std::ifstream oob_file(oob_path);
    if (!oob_file.is_open()) {
        printf("Failed to read units oob file\n");
        assert(false);
    }

    std::string line;
    getline(oob_file, line);

    while (getline(oob_file, line)) {
        if (line == "")
            continue;

        auto parts = split(line, COMMON_SPLIT_CHAR);
        assert(parts.size() == 3);

        auto unit         = deployment->GetUnitById(parts[0]);
        auto weapon_index = armory->GetWeaponIndexById(parts[1]).value();

        unit->weapons.push_back(weapon_index);
        unit->weapons_counter.push_back(std::stoi(parts[2]));
        unit->morale.push_back(1.0);
    }
}

void LoadUnitGeneralInfo(Deployment* deployment, const std::string& general_info_path)
{
    std::ifstream general_info_file(general_info_path);
    if (!general_info_file.is_open()) {
        printf("Failed to read units general info file\n");
        assert(false);
    }

    std::string line;
    getline(general_info_file, line);

    Unit unit;
    std::string deployed_to_oblast;

    while (getline(general_info_file, line)) {
        if (line == "")
            continue;

        auto parts = split(line, COMMON_SPLIT_CHAR);

        unit.id = parts[0];
        unit.name = parts[1];
        unit.nickname = parts[2];
        unit.side = StrToSide(parts[3]);
        unit.commander_index = std::stoi(parts[4]);
        unit.size   = STR_TO_SIZE.at(parts[6]);
        unit.type   = STR_TO_UNIT_TYPE.GetValue(parts[7]);
        unit.stance = STR_TO_UNIT_STANCE.GetValue(parts[8]);

        deployment->Insert(unit, OBLASTS.GetKey(parts[5]));

        // TODO: load commander info
    }
}

Deployment LoadDeployment(Armory* armory, const std::string& path)
{
    auto general_info_path = path + "_general.csv";
    auto oob_info_path = path + "_oob.csv";

    Deployment deployment;

    LoadUnitGeneralInfo(&deployment, general_info_path);
    LoadUnitOOB(armory, &deployment, oob_info_path);

    return deployment;
}
