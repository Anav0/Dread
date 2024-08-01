#include "Fight.h"
#include "GameState.h"
#include "Templates.h"

#include <algorithm>
#include <numeric>
#include <random>

u32 Deployment::Insert(Unit unit, OblastCode code)
{
    units.push_back(unit);
    assigned.push_back(code);
    auto index = units.size() - 1;
    unit_id_by_unit_index.insert({ unit.id, index });
    return index;
}

Unit* Deployment::GetUnitById(const std::string& id)
{
    auto index = unit_id_by_unit_index.at(id);
    return &units.at(index);
}

std::vector<BattleGroup> Fight::FormBattleGroups(OblastCode oblast, Side side, Armory* armory, UnitStance stance, Deployment& deployment)
{

    UnitStance* stances;
    std::vector<Unit>* units;

    std::vector<BattleGroup> battle_groups;
    bool pushed_something_to_group = false;
    u32 i = 0;
    for (auto& unit : deployment.units) {
        auto unit_oblast = deployment.assigned.at(i);

        if (unit.stance != stance || unit.side != side || unit_oblast != oblast) {
            i++;
            continue;
        }

        battle_groups.push_back(FormBattleGroup(armory, i, unit));
        pushed_something_to_group = true;

        i++;
    }

    assert(pushed_something_to_group);
    return battle_groups;
}

void Fight::DissolveBattleGroups(std::vector<BattleGroup>& groups, Deployment& deployment)
{
    for (auto& group : groups) {
        auto& unit = deployment.units.at(group.parent_unit_index);

        for (u32 i = 0; i < group.real_size; i++) {
            auto& w = group.weapons.at(i);
            if (UnitDestroyed(w))
                continue;

            unit.weapons_counter.at(w.index_of_weapon_in_parent_unit)++;
        }
    }
}

std::vector<f32> GetModifiers(SimulationParams& params, WeaponSystemGeneralType type, SideStatus status)
{
    std::vector<f32> mods;
    Modifier modifier = params.modifiers_manager.ua_modifier;
    if (status == SideStatus::Attacking) {
        if (params.attacking_side == Side::RU) {
            modifier = params.modifiers_manager.ru_modifier;
        }
    }

    mods.push_back(status == SideStatus::Defending ? modifier.defense_modifier : modifier.attack_modifier);

    auto current_weather = params.weather_manager.GetWeatherForOblast(params.oblast_code);
    mods.push_back(params.modifiers_manager.GetWeatherModifier(current_weather, type, status));

    auto current_graound_condition = params.weather_manager.GetGroundCondition(params.oblast_code);
    mods.push_back(params.modifiers_manager.GetGroundConditionModifier(current_graound_condition, type, status));

    return mods;
}

AttackResult Fight::SimulateAttack(SimulationParams& params, Armory* armory, Deployment& deployment, SimulationSession* simulation_session = nullptr)
{
    AttackResult result {};

    std::vector<BattleGroup> attacker_battle_grup = FormBattleGroups(params.oblast_code, params.attacking_side, armory, UnitStance::Commited, deployment);
    std::vector<BattleGroup> defender_battle_grup = FormBattleGroups(params.oblast_code, params.defending_side, armory, UnitStance::Defending, deployment);

    u32 group_index = 0;
    for (auto& group : attacker_battle_grup) {
        assert(group.weapons.size() > 0);
        group.Attacking = 1;
        group.Side = SideToStr(params.attacking_side);
        group.Domain = "Ground";
        group.GroupIndex = group_index;
        group_index++;
    }

    group_index = 0;
    for (auto& group : defender_battle_grup) {
        assert(group.weapons.size() > 0);
        group.Attacking = 0;
        group.Side = SideToStr(params.defending_side);
        group.Domain = "Ground";
        group.GroupIndex = group_index;
        group_index++;
    }

    u32 round = 0;

    bool attacker_moral_broke = false;
    bool defender_moral_broke = false;
    bool attacker_was_mauled = false;
    bool defender_was_mauled = false;

    f32 damage_threshold = 0.6;
    f32 moral_threshold = 0.6;

    u16 cqb_rounds = 0;
    while (!defender_moral_broke && !attacker_moral_broke && !attacker_was_mauled && !defender_was_mauled) {

        // Attacking groups go forward!
        if (this->attacker_distance_in_meters > 0) {
            this->attacker_distance_in_meters -= 100;
            if (this->attacker_distance_in_meters < 0)
                this->attacker_distance_in_meters = 0;
        }

        auto def_rounds = Fire(params.defending_side, armory, params, this->attacker_distance_in_meters, defender_battle_grup, attacker_battle_grup);
        auto att_rounds = Fire(params.attacking_side, armory, params, this->attacker_distance_in_meters, attacker_battle_grup, defender_battle_grup);

        if (simulation_session != nullptr) {
            simulation_session->round = round;
            simulation_session->distance_in_meters = this->attacker_distance_in_meters;

            simulation_session->AddRound(def_rounds, SideToStr(params.defending_side));
            simulation_session->AddGroup(defender_battle_grup);
            simulation_session->AddRound(att_rounds, SideToStr(params.attacking_side));
            simulation_session->AddGroup(attacker_battle_grup);
        }

        defender_moral_broke = MoralBroke(defender_battle_grup, moral_threshold);
        defender_was_mauled = AverageDamageExceedsThreshold(defender_battle_grup, damage_threshold);

        attacker_moral_broke = MoralBroke(attacker_battle_grup, moral_threshold);
        attacker_was_mauled = AverageDamageExceedsThreshold(attacker_battle_grup, damage_threshold);

        round++;

        if (attacker_distance_in_meters <= 0) {
            if (cqb_rounds > 10)
                break;
            cqb_rounds++;
        }
    }

    bool defender_won = attacker_moral_broke || attacker_was_mauled;
    bool attacker_won = defender_moral_broke || defender_was_mauled;

    bool draw = (defender_won && attacker_won) || (!defender_won && !attacker_won);

    assert(defender_won || attacker_won || draw);


    DissolveBattleGroups(attacker_battle_grup, deployment);
    DissolveBattleGroups(defender_battle_grup, deployment);

     result.status = AttackResultStatus::Draw;
    if (defender_won)
        result.status = AttackResultStatus::DefenderWon;
    else
        result.status = AttackResultStatus::AttackerWon;
    
    if (simulation_session != nullptr)
        simulation_session->AddResult(result.status);

    return result;
}

static BattleGroup FormBattleGroup(Armory* armory, u32 parent_unit_index, Unit& unit)
{
    BattleGroup group = BattleGroup(armory);

    group.name = "BG from " + unit.name;
    group.parent_unit_index = parent_unit_index;

    assert(unit.weapons.size());

    u32 index = 0;
    for (u32 weapon_index : unit.weapons) {
        if (unit.weapons_counter[index] < 0) {
            assert(false);
            continue;
        }

        WeaponSystem* weapon_ref = &armory->weapons[weapon_index];

        auto& unit_template_n = TEMPLATES.at(unit.type);
        auto desired_size = unit_template_n.at(weapon_ref->type);
        auto total = unit.weapons_counter[index];

        u32 n = 0;
        if (total < desired_size)
            n = total;
        else
            n = desired_size;

        unit.weapons_counter[index] -= n;

        for (u32 i = 0; i < n; i++) {
            group.weapons[group.real_size] = WeaponSystemInGroup();
            group.weapons[group.real_size].weapon = weapon_ref;
            group.weapons[group.real_size].index_of_weapon_in_parent_unit = index;
            group.weapons[group.real_size].morale = 1.0f; // TODO: default for now
            group.real_size++;
        }

        assert(group.real_size < MAX_BG_SIZE);

        index++;
    }

    group.round_info.round = 0;

    return group;
}

bool MoralBroke(std::vector<BattleGroup>& groups, f32 threshold)
{
    auto reducer_morale = [](u32 acc, const WeaponSystemInGroup& w) {
        return acc + w.morale;
    };

    for (BattleGroup& group : groups) {
        auto avg = group.AverageMorale();
        if (avg < threshold)
            return true;
    }

    return false;
};

bool AverageDamageExceedsThreshold(std::vector<BattleGroup>& groups, f32 threshold)
{
    u32 destroyed = 0;
    u32 damaged = 0;
    u32 intact = 0;

    for (auto& group : groups) {
        for (u32 i = 0; i < group.real_size; i++) {
            auto& weapon_system = group.weapons[i];
            if (UnitDestroyed(weapon_system)) {
                destroyed++;
            } else if (UnitDamaged(weapon_system)) {
                damaged++;
            } else {
                intact++;
            }
        }
    }

    printf("Destroyed: %i, Damaged: %i, Intact: %i\n", destroyed, damaged, intact);

    auto v = ((destroyed) / (static_cast<float>(destroyed) + damaged + intact));

    return v > threshold;
};

std::tuple<bool, f32> TryToHitTarget(TargetingInfo& info, std::mt19937& engine, u32 distance)
{
    assert(info.can_fire);

    u32 correct_accuracy_index = 0;
    for(;;) {
        if (correct_accuracy_index > info.ammo_to_use->accuracy.size() - 1)
            break;

        auto& a = info.ammo_to_use->accuracy.at(correct_accuracy_index);

        if (distance > a.range_in_meters) {
            correct_accuracy_index--;
            break;
        }

        correct_accuracy_index++;
    }

    auto& acc = info.ammo_to_use->accuracy.at(correct_accuracy_index);

    if (acc.range_in_meters < distance)
        return { false, 0.0 };

    std::uniform_real_distribution<f32> distribution(0.0, 1.0);

    auto value = distribution(engine);

    return { value < acc.change_to_hit, acc.change_to_hit };
}

PriorityQueue ConstructPriorityQueue(Armory* armory, const std::vector<BattleGroup>& groups)
{
    PriorityQueue queue;

    u32 i = 0;
    u32 j = 0;
    for (auto& g : groups) {
        queue.push_back({});
        for (u32 k = 0; k < g.real_size; k++) {
            u32 priority = 0.0;
            auto& w = g.weapons.at(k);
            auto target_domain = w.weapon->domain;

            if (w.weapon->IsArmored())
                priority += 1;

            if (w.weapon->type == WeaponSystemGeneralType::Tank)
                priority += 2;

            if (w.weapon->IsArtillery())
                priority += 4;

            if (w.weapon->IsAA())
                priority += 4;

            if (UnitDestroyed(w))
                priority = 0;

            queue.at(i).push_back({ j, priority });
            j++;
        }

        std::sort(queue.at(i).begin(), queue.at(i).end(), std::greater {});

        i++;
    }
    return queue;
}

// TODO: Pick targets by priority
TargetingInfo TryTargeting(Armory* armory, const WeaponSystemInGroup& firing_weapon, std::vector<BattleGroup>& enemy_groups, PriorityQueue queue, u32 distance)
{
    TargetingInfo ti {};

    assert(firing_weapon.weapon->max_distance > 0);
    assert(firing_weapon.weapon->max_penetration > 0 || firing_weapon.weapon->max_fragmentation > 0);

    if (!firing_weapon.weapon->CanReach(distance))
        return ti;

    u32 i = 0;

    for (auto& priority_list : queue) {
        auto& targeted_group = enemy_groups.at(i);
        for (auto& priority : priority_list) {
            auto& potential_target_weapon = targeted_group.weapons.at(priority.index);

            if (UnitDestroyed(potential_target_weapon))
                continue;

            if (potential_target_weapon.morale <= 0.0)
                continue;

            if (!firing_weapon.weapon->CanPenetrate(potential_target_weapon.weapon, HitDirection::HullSide))
                continue;

            auto [device, ammo] = firing_weapon.weapon->PickRightDevice(armory, potential_target_weapon.weapon, distance);

            ti.can_fire = true;
            ti.targeted_group = &enemy_groups.at(i);
            ti.targeted_weapon = &enemy_groups.at(i).weapons.at(priority.index);
            ti.ammo_to_use = ammo;
            ti.device_to_use = device;
            ti.weapon_to_use = &firing_weapon;

            return ti;
        }
        i++;
    }

    return ti;
}

std::vector<FireResult> Fire(Side firing_side, Armory* armory, SimulationParams& params, u16 distance_in_m, const std::vector<BattleGroup>& attacking_battlegroups, std::vector<BattleGroup>& targeted_battlegroups)
{
    std::vector<FireResult> results;
    results.reserve(256);

    const std::uniform_int_distribution<u32> d6(0, 6);

    auto queue = ConstructPriorityQueue(armory, targeted_battlegroups);

    for (const BattleGroup& attacking_group : attacking_battlegroups) {
        for (u32 i = 0; i < attacking_group.real_size; i++) {
            auto& attacking_weapon_ref = attacking_group.weapons[i];

            if (!UnitCanFire(attacking_weapon_ref))
                continue;

            auto targeting_info = TryTargeting(armory, attacking_weapon_ref, targeted_battlegroups, queue, distance_in_m);
            if (!targeting_info.can_fire)
                continue;

            FireResult fire_result = FireResult(targeting_info);

            // Hit
            auto [target_was_hit, acc] = TryToHitTarget(targeting_info, params.rnd_engine, distance_in_m);
            if (target_was_hit) {
                fire_result.hit_or_miss = "HIT";
                if (ArmorWasPenetrated(targeting_info)) {
                    // TODO: incorporate more statuses
                    SET_FLAG(targeting_info.targeted_weapon->statuses, WeaponSystemStatus::OnFire);
                }
            } else {
                fire_result.hit_or_miss = "MISS";
            }

            results.push_back(fire_result);
        }
    }

    return results;
}



inline bool WeaponSystem::CanReach(u32 distance_m) const
{
    return distance_m < max_distance;
}

void WeaponSystem::Precompute(Armory* armory)
{
    for (auto device_index : devices) {
        auto& device = armory->devices.at(device_index);

        for (auto& ammo_index : device.ammunition) {
            auto& ammo = armory->ammo.at(ammo_index);

            if (max_distance < ammo.accuracy.at(0).range_in_meters) {
                max_distance = ammo.accuracy.at(0).range_in_meters;
            }

            if (IsAP(&ammo) && ammo.penetration > max_penetration) {
                max_penetration = ammo.penetration;
            }

            if (IsHE(&ammo) && ammo.fragmentation > max_fragmentation) {
                max_fragmentation = ammo.fragmentation;
            }

        }
    }

    assert(max_distance > 0);
}

bool WeaponSystem::CanBePenetrated(Ammo* ammo_used, HitDirection dir) const
{
    return false;
}

inline bool WeaponSystem::CanPenetrate(WeaponSystem* enemy, HitDirection dir) const
{
    auto armor = enemy->GetArmorAt(dir);
    return max_penetration > armor;
}

std::pair<Device*, Ammo*> WeaponSystem::PickRightDevice(Armory* armory, const WeaponSystem* target, u32 distance) const
{
    for (auto device_index : devices) {
         auto device = &armory->devices.at(device_index);
         for (auto ammo_index : device->ammunition) {
             auto ammo = &armory->ammo.at(ammo_index);
             auto in_range = ammo->accuracy.at(0).range_in_meters >= distance;

             if (!in_range)
                 continue;

             if (target->IsArmored() && IsAP(ammo) && target->CanBePenetrated(ammo, HitDirection::HullFront)) {
                 return { device, ammo };
             } 

             if (in_range && ammo->fragmentation > 0)
                 return { device, ammo };
             
         }
    }

    return { nullptr, nullptr };
}

constexpr u32 WeaponSystem::GetArmorAt(HitDirection dir)
{
    switch (dir) {
    case HitDirection::Top:
        return armor.top;
    case HitDirection::HullFront:
        return armor.hull_front;
    case HitDirection::HullSide:
        return armor.hull_side;
    case HitDirection::HullRear:
        return armor.hull_rear;
    case HitDirection::TurretFront:
        return armor.turret_front;
    case HitDirection::TurretSide:
        return armor.turret_side;
    case HitDirection::TurretRear:
        return armor.turret_rear;
    }
    assert(false);
}

inline bool WeaponSystem::IsArmored() const
{
    return this->armor.hull_front > 0;
}

inline bool WeaponSystem::IsArtillery() const
{
    return this->type == WeaponSystemGeneralType::MLRS
        || this->type == WeaponSystemGeneralType::Artillery
        || this->type == WeaponSystemGeneralType::Mortar;
}

inline bool WeaponSystem::IsAA() const
{
    return this->type == WeaponSystemGeneralType::MANPADS;
}

bool IsAP(Ammo* ammo)
{
    return ammo->damage_type == DamageType::Kinetic || ammo->damage_type == DamageType::HEAT || ammo->damage_type == DamageType::Tandem;
}

bool IsHE(Ammo* ammo)
{
    return ammo->damage_type == DamageType::HE || ammo->damage_type == DamageType::HEAT || ammo->damage_type == DamageType::Tandem;
}

// bool WeaponSystem::CanDestroyArmoredTargets(Armory* armory, WeaponDomain domain) const
//{
//     return false;
// }
//
// bool WeaponSystem::CanDestroySoftTargets(Armory* armory, WeaponDomain domain) const
//{
//     for (auto device_index : devices) {
//         auto& device = armory->devices.at(device_index);
//         for (auto ammo_index : device.ammunition) {
//             auto ammo = &armory->ammo.at(ammo_index);
//
//             if (IsHE(ammo)) { }
//
//         }
//     }
//     return false;
// }
