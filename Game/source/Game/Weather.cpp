#include "Weather.h"

#include <cassert>

Weather GetNextWeather(Weather current_weather)
{
    // TODO: come up with algorithm
    switch (current_weather) {
    case Weather::Clear:
        return Weather::Clear;
    case Weather::Cold:
        return Weather::Cold;
    case Weather::Rain:
        return Weather::Rain;
    case Weather::HeavyRain:
        return Weather::HeavyRain;
    case Weather::Snowfall:
        return Weather::Snowfall;
    }

    assert(false);
    return Weather::Clear;
}

GroundCondition GetNextGroundCondition(Weather current_weather)
{
    // TODO: come up with algorithm
    switch (current_weather) {
    case Weather::Clear:
        return GroundCondition::Dry;
    case Weather::Cold:
        return GroundCondition::Frozen;
    case Weather::Rain:
        return GroundCondition::Mud;
    case Weather::HeavyRain:
        return GroundCondition::HeavyMud;
    case Weather::Snowfall:
        return GroundCondition::Frozen;
    }

    assert(false);
    return GroundCondition::Frozen;
}

void UpdateCondition(Condition& condition)
{
    auto tmp1 = condition.current_weather;
    auto tmp2 = condition.current_ground_cond;

    condition.current_weather = GetNextWeather(condition.current_weather);
    condition.current_ground_cond = GetNextGroundCondition(condition.current_weather);

    condition.prev_weather = tmp1;
    condition.prev_ground_cond = tmp2;
}

Weather StrToWeather(const std::string& str)
{
    if (str == "Clear")
        return Weather::Clear;
    if (str == "Cold")
        return Weather::Cold;
    if (str == "Rain")
        return Weather::Rain;
    if (str == "HeavyRain")
        return Weather::HeavyRain;
    if (str == "Snowfall")
        return Weather::Snowfall;
}

std::string WeatherToStr(Weather weather)
{
    switch (weather) {
    case Weather::Clear:
        return "Clear";
    case Weather::Cold:
        return "Cold";
    case Weather::Rain:
        return "Rain";
    case Weather::HeavyRain:
        return "HeavyRain";
    case Weather::Snowfall:
        return "Snowfall";
    }
}

GroundCondition StrToGoundCondition(const std::string& str)
{
    if (str == "Dry")
        return GroundCondition::Dry;
    if (str == "Mud")
        return GroundCondition::Mud;
    if (str == "HeavyMud")
        return GroundCondition::HeavyMud;
    if (str == "Frozen")
        return GroundCondition::Frozen;
}

std::string GroundConditionToStr(GroundCondition condition)
{
    switch (condition) {
    case GroundCondition::Dry:
        return "Dry";
    case GroundCondition::Mud:
        return "Mud";
    case GroundCondition::HeavyMud:
        return "HeavyMud";
    case GroundCondition::Frozen:
        return "Frozen";
    }
}

Weather WeatherManager::GetWeatherForOblast(OblastCode code) const
{
    return this->conditions.at(code).current_weather;
}

Weather WeatherManager::GetPrevWeatherForOblast(OblastCode code) const
{
    return this->conditions.at(code).prev_weather;
}

void WeatherManager::UpdateWeather()
{
    for (auto it = this->conditions.begin(); it != conditions.end(); ++it) {
        UpdateCondition(it->second);
    }
}

GroundCondition WeatherManager::GetGroundCondition(OblastCode code) const
{
    auto& condition = this->conditions.at(code);
    return condition.current_ground_cond;
}

GroundCondition WeatherManager::GetPrevGroundCondition(OblastCode code) const
{
    auto& condition = this->conditions.at(code);
    return condition.prev_ground_cond;
}

void WeatherManager::Init(std::map<OblastCode, std::tuple<Weather, GroundCondition>> req)
{
    for (auto it = req.begin(); it != req.end(); ++it) {
        auto& pair = it->second;

        Condition con;
        con.prev_ground_cond = std::get<1>(pair);
        con.current_ground_cond = std::get<1>(pair);

        con.prev_weather = std::get<0>(pair);
        con.current_weather = std::get<0>(pair);

        this->conditions.insert(std::pair(it->first, con));
    }
}
