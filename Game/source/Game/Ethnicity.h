#ifndef ETHNICITY_H
#define ETHNICITY_H

#pragma once

#include <string>
#include <unordered_map>

#include "Misc/Flags.h"
#include "Misc/Constants.h"

#include "Engine/bimap.h"

enum CurrentRole : u32 {
    Frontline = FLAG(1),
    Support   = FLAG(2),
    Rear      = FLAG(3),
};

enum class Faith : u16 {
    Catholic   = FLAG(1),
    Protestant = FLAG(2),
    Orthodox   = FLAG(3),
    Sunni      = FLAG(4),
    Shia       = FLAG(5),
    Buddist    = FLAG(6),
    Judaism    = FLAG(7),
};

struct Attitude {
    f32 frontline = 1.0; 
    f32 support   = 1.0;      //Artillery
    f32 rear      = 1.0;      //Non combat roles
};

struct Ethnicity {
    char* id;
    char* name;
    Faith faith;
    Attitude attitude;
};

//Speed:
class EthnicityManager {
    static constexpr u16 ETHNICITY_MAX = 64;

    std::array<Ethnicity, ETHNICITY_MAX> ethnicities;

    std::unordered_map<std::string, u32> by_name;
    std::unordered_map<std::string, u32> by_id;
    std::unordered_map<Faith, std::vector<u32>> by_faith;

    f32 GetProperAttitudValue(Ethnicity&, CurrentRole);
    f32 ChangeProperAttitudeValue(Ethnicity&, CurrentRole, f32 by);

public:
    void Load(char* file_path, u64 offset);
    f32 ChangeAttitudeById(char* id, CurrentRole, f32 by);
    f32 ChangeAttitudeByName(char* name, CurrentRole, f32 by);
    f32 GetAttitudeById(char* id, CurrentRole);
    f32 GetAttitudeByName(char* name, CurrentRole);
    Ethnicity* GetById(char* name);
    Ethnicity* GetByName(char* name);
    std::vector<Ethnicity*> GetByFaith(Faith& faith);
};

#endif