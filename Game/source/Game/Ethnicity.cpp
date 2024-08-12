#include "Ethnicity.h"

// void EthnicityManager::Load(char* file_path, u64 offset)
//{
// }

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
