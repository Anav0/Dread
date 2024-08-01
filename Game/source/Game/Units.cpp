#include "Units.h"

std::string DomainToStr(WeaponDomain domain)
{
    const static std::map<WeaponDomain, std::string> map = {
        { WeaponDomain::Cyber, "Cyber" },
        { WeaponDomain::Ground, "Ground" },
        { WeaponDomain::Air, "Air" },
        { WeaponDomain::Sea, "Sea" },
    };

    return map.at(domain);
}

inline WeaponSystemGeneralType StrToWeaponType(std::string& str)
{
    return WEAPON_SYSTEM_GENERAL_TYPE_MAP.GetKey(str);
}

inline std::string WeaponTypeToStr(WeaponSystemGeneralType type)
{
    return WEAPON_SYSTEM_GENERAL_TYPE_MAP.GetValue(type);
}

void PrintUnit(Armory& armory, Unit& unit)
{
    std::cout << "\nName: " << unit.name << " ";
    std::cout << "Nickname: " << unit.nickname << " ";
    std::cout << "Size: " << static_cast<int>(unit.size) << " ";
    std::cout << "Side: " << static_cast<int>(unit.side) << " ";

    u32 index = 0;
    for (auto& weapon : unit.weapons) {
        std::cout << "\n\t";
        std::cout << armory.weapons[weapon.index_in_armory].name << ", ";
        std::cout << weapon.n << " out of ";
        std::cout << weapon.initial_n;
        index++;
    }
}

bool IsAP(Ammo*);
bool IsHE(Ammo*);

