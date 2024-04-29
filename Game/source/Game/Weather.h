#pragma once

#include "Fight.h"
#include "Entities.h"

#include <map>

enum class GroundCondition {
    Dry,
    Mud,
    HeavyMud,
    Frozen,
};

enum class Weather {
    Clear,
    Cold,
    Rain,
    HeavyRain, // Jaaaaassssssooooonnnnn!!!
    Snowfall,
};

struct Condition {
    Weather prev_weather;
    Weather current_weather;

    GroundCondition prev_ground_cond;
    GroundCondition current_ground_cond;
};

Weather GetNextWeather(Weather current_weather);
GroundCondition GetNextGroundCondition(Weather current_weather);
void UpdateCondition(Condition& condition);

class WeatherManager {
private:
    std::map<OblastCode, Condition> conditions;

public:
    Weather GetWeatherForOblast(OblastCode);
    Weather GetPrevWeatherForOblast(OblastCode);

    void UpdateWeather();
};
