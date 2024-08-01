#include "Ethnicity.h"

// void EthnicityManager::Load(char* file_path, u64 offset)
//{
// }

f32 EthnicityManager::GetProperAttitudValue(Ethnicity& ethnicty, CurrentRole role)
{
    switch (role) {
    case CurrentRole::Frontline:
        return ethnicty.attitude.frontline;
    case CurrentRole::Rear:
        return ethnicty.attitude.rear;
    case CurrentRole::Support:
        return ethnicty.attitude.support;
    }

    assert(false);
    return 1.0;
}

f32 EthnicityManager::ChangeProperAttitudeValue(Ethnicity& ethnicity, CurrentRole role, f32 by)
{
    switch (role) {
    case CurrentRole::Frontline:
        ethnicity.attitude.frontline += by;
        return ethnicity.attitude.frontline;
    case CurrentRole::Rear:
        ethnicity.attitude.rear += by;
        return ethnicity.attitude.rear;
    case CurrentRole::Support:
        ethnicity.attitude.support += by;
        return ethnicity.attitude.support;
    }

    assert(false);
    return 1.0;
}

f32 EthnicityManager::ChangeAttitudeById(char* id, CurrentRole role, f32 by)
{
    auto index      = by_id.at(id);
    auto& ethnicity = ethnicities.at(index);
    return ChangeProperAttitudeValue(ethnicity, role, by);
}

f32 EthnicityManager::ChangeAttitudeByName(char* name, CurrentRole role, f32 by)
{
    auto index      = by_name.at(name);
    auto& ethnicity = ethnicities.at(index);
    return ChangeProperAttitudeValue(ethnicity, role, by);
}

f32 EthnicityManager::GetAttitudeById(char* id, CurrentRole role)
{
    auto index      = by_id.at(id);
    auto& ethnicity = ethnicities.at(index);

    return GetProperAttitudValue(ethnicity, role);
}

f32 EthnicityManager::GetAttitudeByName(char* name, CurrentRole role)
{
    auto index      = by_name.at(name);
    auto& ethnicity = ethnicities.at(index);

    return GetProperAttitudValue(ethnicity, role);
}

Ethnicity* EthnicityManager::GetById(char* id)
{
    if (!by_id.contains(id))
        return nullptr;

    u32 index = by_id.at(id);
    return &ethnicities.at(index);
}

Ethnicity* EthnicityManager::GetByName(char* name)
{
    if (!by_name.contains(name))
        return nullptr;

    u32 index = by_name.at(name);
    return &ethnicities.at(index);
}

std::vector<Ethnicity*> EthnicityManager::GetByFaith(Faith& faith)
{
    if (!by_faith.contains(faith))
        return {};

    auto& indexes = by_faith.at(faith);

    std::vector<Ethnicity*> output;

    for (auto index : indexes)
        output.push_back(&ethnicities.at(index));

    return output;
}
