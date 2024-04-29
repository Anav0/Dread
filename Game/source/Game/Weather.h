#pragma once

#include "Entities.h"
#include "Fight.h"

#include <map>
#include <string>
#include <tuple>

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

Weather StrToWeather(const std::string&);
std::string WeatherToStr(Weather);

GroundCondition StrToGoundCondition(const std::string&);
std::string GroundConditionToStr(GroundCondition);

class WeatherManager {
private:
    std::map<OblastCode, Condition> conditions;

public:
    void Init(std::map<OblastCode, std::tuple<Weather, GroundCondition>> conditions);
    Weather GetWeatherForOblast(OblastCode);
    Weather GetPrevWeatherForOblast(OblastCode);

    void UpdateWeather();
};
