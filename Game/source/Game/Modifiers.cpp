#include "Modifiers.h"

// TODO: Implement
HitDirection DeterminHitDirection(TargetingInfo& targeting_info)
{
    return HitDirection::HullFront;
}

bool ArmorWasPenetrated(TargetingInfo& targeting_info)
{
    HitDirection dir = DeterminHitDirection(targeting_info);
    u32 armor_value = targeting_info.targeted_weapon->weapon->GetArmorAt(dir);
    return armor_value < targeting_info.ammo_to_use->penetration;
}

void ModifiersManager::AddModifier(Side side, std::string name, ModifierFn fn) {
    modifiers.push_back(Modifier(side, name, fn));
}
