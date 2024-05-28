#include <Game/Devices.h>
#include <Game/Fight.h>
#include <Game/Modifiers.h>
#include <Game/Weather.h>

#include "Misc/CsvSaver.h"

#include "Problem.h"

const char* units_path = "D:/Projects/Dread/Game/data/units.csv";
const char* armory_path = "D:/Projects/Dread/Game/data/weapons.csv";
const char* storage_path = "D:/Projects/Dread/Game/data/storage.csv";
const char* conditions_path = "D:/Projects/Dread/Game/data/conditions.csv";

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

constexpr u32 MAX_RUNS = 10;

// TODO: copy pasta from Devices.cpp
std::vector<std::string> split2(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::map<OblastCode, std::tuple<Weather, GroundCondition>> GetInitialConditions2()
{
    std::map<OblastCode, std::tuple<Weather, GroundCondition>> conditions;
    for (auto it = INITIAL_CONTROL.begin(); it != INITIAL_CONTROL.end(); ++it) {
        conditions.insert({ it->first, { Weather::Clear, GroundCondition::Dry } });
    }
    return conditions;
}

int main(int argc, char* argv[])
{
    assert(argc == 2);

    Fill();

    auto save_only_this_arr = split2(argv[1], ';');
    auto save_only_this = std::set<std::string>(save_only_this_arr.begin(), save_only_this_arr.end());

    Armory armory         = LoadArmory(armory_path, storage_path);
    Deployment deployment = LoadUnits(armory.weapons, OBLASTS, units_path);

    SimulationSession session = SimulationSession(&armory, deployment, save_only_this);

    WeatherManager weather_manager = WeatherManager();
    weather_manager.Init(GetInitialConditions2());
    ModifiersManager modifiers_manager = ModifiersManager();
    modifiers_manager.LoadWeatherModifiers(conditions_path);
    modifiers_manager.ru_modifier = Modifier(0.55, 1.0);
    modifiers_manager.ua_modifier = Modifier(1.1, 1.25);

    SimulationParams params = SimulationParams(OblastCode::Donetsk, Side::RU, weather_manager, modifiers_manager);

    Fight fight;

    for (u32 run = 0; run < MAX_RUNS; run++) {
        if (true) {
            printf("%i/%i\r", run, MAX_RUNS);
        }

        fight.attacker_distance_in_meters = 6000;

        fight.ua_units[0] = 0;
        fight.ua_stance[0] = UnitStance::Defending;

        fight.ru_units[0] = 0;
        fight.ru_stance[0] = UnitStance::Committed;

        session.run = run;

        Armory armory_cpy = Armory(armory);
        Deployment deployment_cpy = Deployment(deployment);

        session.armory = &armory_cpy;
        session.deployment = deployment_cpy;

        assert(armory_cpy.weapons.size() == armory.weapons.size());

        fight.SimulateAttack(params, &armory_cpy, deployment_cpy, &session);
    }

    printf("Flush!\n");
    session.Flush();
    printf("Close!\n");
    session.Close();

    return 0;
}