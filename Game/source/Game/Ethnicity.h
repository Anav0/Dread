#ifndef ETHNICITY_H
#define ETHNICITY_H

#pragma once

#include <string>
#include <array>
#include <unordered_map>

#include "Misc/Flags.h"
#include "Misc/Constants.h"

#include "Engine/bimap.h"

enum class Faith : u16 {
    Catholic   = FLAG(1),
    Protestant = FLAG(2),
    Orthodox   = FLAG(3),
    Sunni      = FLAG(4),
    Shia       = FLAG(5),
    Buddist    = FLAG(6),
    Judaism    = FLAG(7),
};

struct Ethnicity {
    char* id;
    char* name;
    Faith faith;
};

//Speed:
class EthnicityManager {
    static constexpr u16 ETHNICITY_MAX = 64;

    std::array<Ethnicity, ETHNICITY_MAX> ethnicities;

    std::unordered_map<std::string, u32>        by_name;
    std::unordered_map<std::string, u32>        by_id;
    std::unordered_map<Faith, std::vector<u32>> by_faith;

public:
    void Load(char* file_path, u64 offset);
    Ethnicity* GetById(char* name);
    Ethnicity* GetByName(char* name);
    std::vector<Ethnicity*> GetByFaith(Faith& faith);
};

#endif
