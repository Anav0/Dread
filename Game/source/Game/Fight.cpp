#include "Fight.h"
#include "GameState.h"

#include <algorithm>
#include <numeric>
#include <random>

Unit* Deployment::GetUnitById(std::string_view id)
{
    for (auto& unit : units) {
        if (unit.id == id) {
            return &unit;
        }
    }
}

std::vector<BattleGroup> Fight::FormBattleGroups(Side side, Armory* armory, UnitStance stance, Deployment& deployment)
{
    UnitStance* stances;
    i32* unit_indexes;
    std::vector<Unit>* units;

    if (side == Side::UA) {
        stances = this->ua_stance;
        unit_indexes = this->ua_units;
        units = &deployment.ukr_units;
    } else {
        stances = this->ru_stance;
        unit_indexes = this->ru_units;
        units = &deployment.ru_units;
    }

    std::vector<BattleGroup> battle_groups;
    bool pushed_something_to_group = false;
    for (u8 i = 0; i < MAX_UNITS; i++) {
        UnitStance ua_unit_stance = stances[i];
        if (ua_unit_stance == stance) {
            i32 unit_index = unit_indexes[i];
            if (unit_index != -1) {
                Unit& unit = units->at(unit_index);
                battle_groups.push_back(FormBattleGroup(armory, unit_index, unit));
                pushed_something_to_group = true;
            }
        }
    }
    assert(pushed_something_to_group);
    return battle_groups;
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
    AttackResult result;

    std::vector<BattleGroup> attacker_battle_grup = FormBattleGroups(params.attacking_side, armory, UnitStance::Committed, deployment);
    std::vector<BattleGroup> defender_battle_grup = FormBattleGroups(params.defending_side, armory, UnitStance::Defending, deployment);

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

        assert(round < 10000);
    }

    bool defender_won = attacker_moral_broke || attacker_was_mauled;
    bool attacker_won = defender_moral_broke || defender_was_mauled;

    bool draw = (defender_won && attacker_won) || (!defender_won && !attacker_won);

    assert(defender_won || attacker_won || draw);

    // TODO: deal with draws
    if (!draw && simulation_session != nullptr)
        simulation_session->AddWinner(defender_won ? params.defending_side : params.attacking_side);

    result.winner_side = defender_won ? params.defending_side : params.attacking_side;
    result.winner_status = defender_won ? SideStatus::Defending : SideStatus::Attacking;

    return result;
}

static BattleGroup FormBattleGroup(Armory* armory, u32 parent_unit_index, Unit& unit)
{
    BattleGroup group = BattleGroup(armory);

    group.name = "BG from " + unit.name;
    group.parent_unit_index = parent_unit_index;

    u32 index = 0;
    for (u32 weapon_index : unit.weapons) {
        if (unit.weapons_counter[index] < 0) {
            assert(false);
            continue;
        }

        WeaponSystem* weapon_ref = &armory->weapons[weapon_index];

        u32 n = unit.weapons_counter[index] * 0.8;
        unit.weapons_counter[index] -= n;

        for (u32 i = 0; i < n; i++) {
            group.weapons[group.real_size] = WeaponInGroup();
            group.weapons[group.real_size].weapon = weapon_ref;
            // group.weapons[group.real_size].initial_state = weapon_ref->default_state;
            // group.weapons[group.real_size].state = weapon_ref->default_state;
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
    auto reducer_morale = [](u32 acc, const WeaponInGroup& w) {
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
            if (weapon_system.state <= 0) {
                destroyed++;
            } else if (weapon_system.state < weapon_system.initial_state) {
                damaged++;
            } else {
                intact++;
            }
        }
    }

    auto v = ((destroyed) / (static_cast<float>(destroyed) + damaged + intact));

    return v > threshold;
};

std::tuple<bool, f32> TryToHitTarget(TargetingInfo& info, std::mt19937& engine, u32 distance)
{
    assert(info.can_fire);

    u32 index = 0;
    Accuracy acc = info.ammo_to_use->accuracy[0];
    while (distance < acc.range_in_meters) {
        index++;
        if (index > info.ammo_to_use->accuracy.size() - 1)
            break;
        acc = info.ammo_to_use->accuracy[index];
    }

    if (acc.range_in_meters < distance)
        return { false, 0.0 };

    std::uniform_real_distribution<f32> distribution(0.0, 1.0);

    auto value = distribution(engine);

    return { value < acc.change_to_hit, acc.change_to_hit };
}

// TODO: fill
TargetingInfo TryPickingTarget(const WeaponInGroup& firing_weapon, std::mt19937& mt, std::vector<BattleGroup>& enemy_groups, u32 distance)
{
    TargetingInfo ti;

    for (auto& group : enemy_groups) {
        for (auto& w : group.weapons) {
        }
    }

    return ti;
}

std::vector<FireResult> Fire(Side firing_side, Armory* armory, SimulationParams& params, u16 distance_in_m, const std::vector<BattleGroup>& attacking_battlegroups, std::vector<BattleGroup>& targeted_battlegroups)
{
    std::vector<FireResult> results;
    results.reserve(256);

    const std::uniform_int_distribution<u32> d6(0, 6);

    for (const BattleGroup& attacking_group : attacking_battlegroups) {
        for (u32 i = 0; i < attacking_group.real_size; i++) {
            auto& attacking_weapon_ref = attacking_group.weapons[i];

            const bool is_attacking_weapon_disabled = attacking_weapon_ref.state <= 0;

            if (is_attacking_weapon_disabled)
                continue;

            TargetingInfo targeting_info = TryPickingTarget(attacking_weapon_ref, params.rnd_engine, targeted_battlegroups, distance_in_m);
            if (!targeting_info.can_fire)
                continue;

            FireResult fire_result = FireResult(targeting_info);
            fire_result.starting_state = targeting_info.targeted_weapon->state;

            // Hit
            auto [target_was_hit, acc] = TryToHitTarget(targeting_info, params.rnd_engine, distance_in_m);
            if (target_was_hit) {
                fire_result.status = "HIT";
                // NOTE: for now all penetrating damage result in destruction of the equipment
                if (ArmorWasPenetrated(targeting_info)) {
                    targeting_info.targeted_weapon->state = 0.0;
                }
            } else {
                fire_result.status = "MISS";
            }

            results.push_back(fire_result);
        }
    }

    return results;
}

void PrintUnit(Armory& armory, Unit& unit)
{
    std::cout << "\nName: " << unit.name << " ";
    std::cout << "Nickname: " << unit.nickname << " ";
    std::cout << "Size: " << static_cast<int>(unit.size) << " ";
    std::cout << "Side: " << static_cast<int>(unit.side) << " ";

    u32 index = 0;
    for (u32 weapon_index : unit.weapons) {
        std::cout << "\n\t";
        std::cout << armory.weapons[weapon_index].name << ", ";
        std::cout << unit.weapons_counter[index] << " out of ";
        std::cout << unit.weapons_toe[index];
        index++;
    }
}

std::string DomainToStr(WeaponDomain domain)
{
    const static std::map<WeaponDomain, std::string> map = {
        { WeaponDomain::Cyber, "Cyber" },
        { WeaponDomain::Ground, "Ground" },
        { WeaponDomain::Air, "Air" },
        { WeaponDomain::Sea, "Sea" },
    };

    return map.at(domain);
}

u32 WeaponSystem::GetArmorAt(HitDirection dir)
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

bool WeaponSystem::IsArmored()
{
    return this->armor.hull_front > 0;
}

bool IsAP(Ammo* ammo)
{
    return ammo->damage_type == DamageType::Kinetic || ammo->damage_type == DamageType::HEAT || ammo->damage_type == DamageType::Tandem;
}

bool IsHE(Ammo* ammo)
{
    return ammo->damage_type == DamageType::HE || ammo->damage_type == DamageType::HEAT || ammo->damage_type == DamageType::Tandem;
}
