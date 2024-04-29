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

    condition.current_weather     = GetNextWeather(condition.current_weather);
    condition.current_ground_cond = GetNextGroundCondition(condition.current_weather);

    condition.prev_weather = tmp1;
    condition.prev_ground_cond = tmp2;
}

Weather WeatherManager::GetWeatherForOblast(OblastCode code)
{
    return this->conditions.at(code).current_weather;
}

Weather WeatherManager::GetPrevWeatherForOblast(OblastCode code)
{
    return this->conditions.at(code).prev_weather;
}

void WeatherManager::UpdateWeather()
{
    for (auto it = this->conditions.begin(); it != conditions.end(); ++it) {
        UpdateCondition(it->second);
    }
}
