#include "GameState.h"
#include "Modifiers.h"

std::map<OblastCode, std::tuple<Weather, GroundCondition>> GetInitialConditions()
{
    std::map<OblastCode, std::tuple<Weather, GroundCondition>> conditions;
    for (auto it = INITIAL_CONTROL.begin(); it != INITIAL_CONTROL.end(); ++it) {
        conditions.insert({ it->first, { Weather::Clear, GroundCondition::Dry } });
    }
    return conditions;
}

void AddModifiers(ModifiersManager& manager) {
 //    manager.AddModifier(Side::RU, "Weather", [] (Armory*, const WeaponSystemInGroup&, AffectedParams& affected) {
	// 	auto weather = affected.sim_params.weather_manager.GetWeatherForOblast(affected.sim_params.oblast_code);
	// 	//TODO: use table constructed from csv file to get modifiers per unit
	// 	//type
	// 	switch(weather) {
	// 		case Weather::Clear:
	// 		case Weather::Cold:
	// 		case Weather::Rain:
	// 		case Weather::HeavyRain:
	// 		case Weather::Snowfall:
	// 			break;
	// 	};
 //    });

 //    manager.AddModifier(Side::RU, "RU default", [] (Armory*, const WeaponSystemInGroup&, AffectedParams& affected) {
	// 	if(affected.sim_params.attacking_side == Side::RU) {
	// 		result.attack *= 0.6;
	// 	}
	// 	if(affected.sim_params.defending_side == Side::RU) {
	// 		result.defense *= 0.8;
	// 	}
 //    });

	// manager.AddModifier(Side::UA, "UA default", [] (Armory*, const WeaponSystemInGroup&, AffectedParams& affected) {
	// 	if(affected.sim_params.attacking_side == Side::UA) {
	// 		result.attack *= 1.0;
	// 	}
	// 	if(affected.sim_params.defending_side == Side::UA) {
	// 		result.defense *= 1.0;
	// 	}
 //    });

}

GameState LoadGame() {
    GameState state = {};

    const char* armory_path     = "D:/Projects/Dread/Game/data/weapons.csv";
    const char* storage_path    = "D:/Projects/Dread/Game/data/simulation.csv";
    const char* ammo_path       = "D:/Projects/Dread/Game/data/ammo.csv";
    const char* devices_path    = "D:/Projects/Dread/Game/data/devices.csv";
    const char* units_path      = "D:/Projects/Dread/Game/data/units.csv";
    const char* conditions_path = "D:/Projects/Dread/Game/data/conditions.csv";

    state.armory            = LoadArmory(armory_path, ammo_path, devices_path, storage_path);
    state.troops_deployment = LoadDeployment(&STATE.armory, units_path);
    state.weather_manager   = WeatherManager();

    state.weather_manager.Init(GetInitialConditions());
    state.modifiers_manager = ModifiersManager();

    AddModifiers(STATE.modifiers_manager);

    return state;
}
