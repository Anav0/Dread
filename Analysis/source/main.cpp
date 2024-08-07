#include <Game/Devices.h>
#include <Game/Fight.h>
#include <Game/Modifiers.h>
#include <Game/Weather.h>

// #include "Misc/CsvSaver.h"

// #include "Problem.h"

#include "Tests.h"
#include "Tests.cpp"

const char* units_path       = "D:/Projects/Dread/Game/data/units";
const char* weapons_path     = "D:/Projects/Dread/Game/data/weapons.csv";
const char* devices_path     = "D:/Projects/Dread/Game/data/devices.csv";
const char* ammo_path        = "D:/Projects/Dread/Game/data/ammo.csv";
const char* storage_path     = "D:/Projects/Dread/Game/data/storage.csv";
const char* conditions_path  = "D:/Projects/Dread/Game/data/conditions.csv";

constexpr u32 MAX_RUNS = 50;

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
#if DEBUG
    RunTests();
#endif

    assert(argc == 2);

    auto save_only_this_arr = split2(argv[1], ';');
    auto save_only_this = std::set<std::string>(save_only_this_arr.begin(), save_only_this_arr.end());

    Armory armory         = LoadArmory(weapons_path, ammo_path, devices_path, storage_path);
    Deployment deployment = LoadDeployment(&armory, units_path);

    SimulationSession session = SimulationSession(&armory, deployment, save_only_this);

    WeatherManager weather_manager = WeatherManager();
    weather_manager.Init(GetInitialConditions2());
    ModifiersManager modifiers_manager = ModifiersManager();
    modifiers_manager.LoadWeatherModifiers(conditions_path);
    modifiers_manager.ru_modifier = Modifier(2.55, 1.0);
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
        fight.ru_stance[0] = UnitStance::Commited;

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
