#include <Game/Devices.h>
#include <Game/Fight.h>
#include <Game/Modifiers.h>
#include <Game/Weather.h>
#include <Game/Modifiers.h>

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

#ifdef ANALYSIS_MODE

void AddModifiers(ModifiersManager& manager) {
    manager.AddModifier(Side::RU, "Weather", [] (Armory*, const WeaponSystemInGroup&, CombatParams& affected) {
		auto weather = affected.sim_params.weather_manager.GetWeatherForOblast(affected.sim_params.oblast_code);
		//TODO: use table constructed from csv file to get modifiers per unit
		//type
		switch(weather) {
			case Weather::Clear:
			case Weather::Cold:
			case Weather::Rain:
			case Weather::HeavyRain:
			case Weather::Snowfall:
				break;
		};
    });

    manager.AddModifier(Side::RU, "RU default", [] (Armory*, const WeaponSystemInGroup&, CombatParams& affected) {
		if(affected.sim_params.attacking_side == Side::RU) {
		}
		if(affected.sim_params.defending_side == Side::RU) {
		}
    });

    manager.AddModifier(Side::UA, "UA default", [] (Armory*, const WeaponSystemInGroup&, CombatParams& affected) {
        if(affected.sim_params.attacking_side == Side::UA) {
        }
        if(affected.sim_params.defending_side == Side::UA) {
        }
    });

}

#endif

int main(int argc, char* argv[])
{
#ifdef DEBUG
    RunTests();
#endif
    #ifdef ANALYSIS
        printf("ANALYSIS!\n");
    #endif
    #ifdef DEBUG
        printf("DEBUG!\n");
    #endif
    #ifdef RELEASE
        printf("RELEASE!\n");
    #endif
    #ifdef ANALYSIS_MODE
        printf("ANALYSIS_MODE!\n");
    #endif
        printf("CONTROL!\n");

    assert(argc == 2);

    auto save_only_this_arr = split2(argv[1], ':');
    auto save_only_this = std::set<std::string>(save_only_this_arr.begin(), save_only_this_arr.end());

    Armory armory         = LoadArmory(weapons_path, ammo_path, devices_path, storage_path);
    Deployment deployment = LoadDeployment(&armory, units_path);

    SimulationSession session = SimulationSession(&armory, deployment, save_only_this);

    WeatherManager weather_manager = WeatherManager();
    weather_manager.Init(GetInitialConditions2());
    ModifiersManager modifiers_manager = ModifiersManager();

    AddModifiers(modifiers_manager);

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
