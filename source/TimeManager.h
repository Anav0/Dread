#pragma once

#include <chrono>
#include <map>
#include <string>

using namespace std::chrono;

class TimeManager {

    std::map<std::string, system_clock::time_point> time_checkpoints;

public:
    void TrySaveFromNow(std::string key, seconds dur)
    {
        auto time = system_clock::now() + dur;

        TrySavingTimepoint(key, time);
    }

    void TrySavingTimepoint(std::string key, system_clock::time_point time)
    {
        if (time_checkpoints.contains(key))
            return;

        time_checkpoints.insert(std::pair(key, time));
    }

    system_clock::time_point GetTimepoint(std::string key)
    {
        return time_checkpoints.at(key);
    }

    void ClearTimepoint(std::string key)
    {
        time_checkpoints.erase(key);
    }
};
