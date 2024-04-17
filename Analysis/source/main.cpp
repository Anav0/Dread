#include <Game/Devices.h>
#include <Game/Fight.h>

#include "Misc/CsvSaver.h"

const char* units_path = "D:/Projects/Dread/Game/data/units.csv";
const char* armory_path = "D:/Projects/Dread/Game/data/weapons.csv";
const char* storage_path = "D:/Projects/Dread/Game/data/storage.csv";

/*

Phases:

        0. During one turn attacker can perform n number of attacks, based on his determination
            0a. Each attack look like this:
        1. Attacker forms battlegroups
        2. Defender can fire at the gathering point if he has assets (aircraft, drones and artillery)
        3. Battlegroups then move towards defenders positions
        4. Assets on both sides fire if the range permits
                5a. If the attackers morale breakes he tries to retreat
                5b. If the defenders morale breakes they give away their positions
                        5ba. Defender can counterattack to try to recapture lost positions
        6.

*/

static BiMap<OblastCode, const std::string> OBLASTS;

static void Fill()
{
    OBLASTS.Insert(OblastCode::Zythomyr, "Zythomyr");
    OBLASTS.Insert(OblastCode::Zaporizhia, "Zaporizhia");
    OBLASTS.Insert(OblastCode::Zakarpatia, "Zakarpatia");
    OBLASTS.Insert(OblastCode::Volyn, "Volyn");
    OBLASTS.Insert(OblastCode::Vinnytsia, "Vinnytsia");
    OBLASTS.Insert(OblastCode::Ternopil, "Ternopil");
    OBLASTS.Insert(OblastCode::Sumy, "Sumy");
    OBLASTS.Insert(OblastCode::Rivne, "Rivne");
    OBLASTS.Insert(OblastCode::Poltava, "Poltava");
    OBLASTS.Insert(OblastCode::Odessa, "Odessa");
    OBLASTS.Insert(OblastCode::Mykolaiv, "Mykolaiv");
    OBLASTS.Insert(OblastCode::Lviv, "Lviv");
    OBLASTS.Insert(OblastCode::Luhansk, "Luhansk");
    OBLASTS.Insert(OblastCode::Kirovohrad, "Kirovohrad");
    OBLASTS.Insert(OblastCode::Kiev, "Kiev");
    OBLASTS.Insert(OblastCode::Chmielnicki, "Chmielnicki");
    OBLASTS.Insert(OblastCode::Kherson, "Kherson");
    OBLASTS.Insert(OblastCode::Kharkiv, "Kharkiv");
    OBLASTS.Insert(OblastCode::IvanoFrankivsk, "IvanoFrankivsk");
    OBLASTS.Insert(OblastCode::Donetsk, "Donetsk");
    OBLASTS.Insert(OblastCode::Dnipropetrovsk, "Dnipropetrovsk");
    OBLASTS.Insert(OblastCode::Crimea, "Crimea");
    OBLASTS.Insert(OblastCode::Chernihiv, "Chernihiv");
    OBLASTS.Insert(OblastCode::Chernivtsi, "Chernivtsi");
    OBLASTS.Insert(OblastCode::Cherkasy, "Cherkasy");
}

/*
    Header file

    Instance ID, UA Weapons (Soft, Hard, N), RU Weapons (Soft, Hard, N)

    ---

    File A

    We simulate k fights starting at l distance

    Each line in data file looks like this:

    # Iter Status Weapon Device ACC TargetWeapon StartingState Dmg StateAfterHit Distance
    0    HIT   BMP2   2A42   0.5 BMP1 100 24 76 2400
    ...
    k-1 HIT   BMP2   2A42   0.5 BMP1 100 24 76 2400
    k   MISS Javelin Missle 0.5 T72  100 360 0 1400

    ---

    File B

    Winner UA_menlost UA_tank_lost UA_ifv_lost UA_cars_lost UA_drones_lost ... RU_tank_lost etc etc
    UA 1023 20 10
*/

struct Person {
    std::string name, gender;
    u32 age;

    std::string ToCsvRow() const {
        return name + ";" + gender + ";" + std::to_string(age);
    }
};
int main()
{
    Fill();

    CsvSaver saver = CsvSaver("./test.csv");

    Armory armory         = LoadArmory(armory_path, storage_path);
    Deployment deployment = LoadUnits(armory.weapons, OBLASTS, units_path);

    Fight fight;
    fight.saver = &saver;
    fight.attacker_distance_in_meters = 6000;
    fight.phase = 0;
    fight.ua_units[0] = 0;
    fight.ua_stance[0] = UnitStance::Defending;

    fight.ru_units[0] = 0;
    fight.ru_stance[0] = UnitStance::Committed;
    fight.ru_units[1] = 1;

    fight.SimulateAttack(armory, deployment);

    return 0;
}