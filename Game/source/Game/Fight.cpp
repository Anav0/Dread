#include "Fight.h"
#include "GameState.h"

#include <algorithm>
#include <numeric>
#include <random>

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

        // DEFENDER FIRE
        auto def_rounds = Fire(params.defending_side, armory, params, this->attacker_distance_in_meters, defender_battle_grup, attacker_battle_grup);

        // ATTACKER FIRE
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
        group.weapons.push_back(weapon_ref);
        u32 n = unit.weapons_counter[index] * 0.8;
        unit.weapons_counter[index] -= n;
        group.weapons_counter.push_back(n);
        group.weapons_state.push_back(weapon_ref->default_state);
        group.initial_weapons_state.push_back(weapon_ref->default_state);
        group.morale.push_back(unit.morale[index]);
        index++;
    }

    group.round_info.round = 0;

    return group;
}

bool MoralBroke(std::vector<BattleGroup>& groups, f32 threshold)
{
    for (BattleGroup& group : groups) {
        f32 avg_morale = std::reduce(group.morale.begin(), group.morale.end()) / group.morale.size();
        if (avg_morale < threshold)
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
        u32 index = 0;
        for (auto& weapon_system : group.weapons) {
            if (group.weapons_state.at(index) <= 0) {
                destroyed++;
            } else if (group.weapons_state.at(index) < group.initial_weapons_state.at(index)) {
                damaged++;
            } else {
                intact++;
            }
            index++;
        }
    }

    auto v = ((destroyed) / (static_cast<float>(destroyed) + damaged + intact));

    return v > threshold;
};

std::tuple<bool, f32> TryToHitTarget(Ammo& ammo, u32 distance)
{
    u32 index = 0;
    Accuracy acc = ammo.accuracy[0];
    while (distance < acc.range_in_meters) {
        index++;
        if (index > ammo.accuracy.size() - 1)
            break;
        acc = ammo.accuracy[index];
    }

    std::random_device rd;
    std::uniform_real_distribution<f32> distribution(0.0, 1.0);
    std::mt19937 engine(rd());

    auto value = distribution(engine);

    return { value < acc.change_to_hit, acc.change_to_hit };
}

std::tuple<u32, WeaponSystem*, BattleGroup&> PickTarget(std::uniform_int_distribution<u32> dist, std::mt19937 mt, std::vector<BattleGroup>& enemy_groups)
{
    u32 index = dist(mt);
    BattleGroup& target_group = enemy_groups.at(index);

    dist = std::uniform_int_distribution<u32>(0, target_group.weapons.size() - 1);

    index = dist(mt);

    return { index, target_group.weapons.at(index), target_group };
}

static Ammo& PickRightAmmunitionForTarget(Armory* armory, Device& firing_device, WeaponSystem* target)
{
    assert(target != nullptr);
    assert(firing_device.ammunition.size() > 0);

    return armory->ammo.at(*firing_device.ammunition.begin());
}

std::vector<FireResult> Fire(Side firing_side, Armory* armory, const SimulationParams& params, u16 distance_in_m, const std::vector<BattleGroup>& attacking_battlegroups, std::vector<BattleGroup>& targeted_battlegroups)
{
    std::vector<FireResult> results;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<u32> dist(0, targeted_battlegroups.size() - 1);
    std::uniform_int_distribution<u32> d6(0, 6);

    for (const BattleGroup& attacking_group : attacking_battlegroups) {
        u32 attacking_weapon_index = 0;
        for (WeaponSystem* attacking_weapon_ptr : attacking_group.weapons) {
            f32 attacking_weapon_state = attacking_group.weapons_state.at(attacking_weapon_index);

            const bool is_attacking_weapon_disabled = attacking_weapon_state <= 0;
            const u32 no_more_weapon_left = attacking_group.weapons_counter.at(attacking_weapon_index) <= 0;

            if (no_more_weapon_left || is_attacking_weapon_disabled)
                continue;

            for (u32 device_index : attacking_weapon_ptr->devices) {
                auto& attacking_device = armory->devices.at(device_index);

                auto [targeted_weapon_index_in_group, targeted_weapon, targeted_group] = PickTarget(dist, mt, targeted_battlegroups);
                auto& ammunition_used_for_attack = PickRightAmmunitionForTarget(armory, attacking_device, targeted_weapon);

                // TODO: temporary
                f32 targeted_weapon_state = targeted_group.weapons_state.at(targeted_weapon_index_in_group);

                const bool is_out_of_range = ammunition_used_for_attack.accuracy[0].range_in_meters < distance_in_m;
                const bool is_not_of_the_same_domain = (targeted_weapon->domain != ammunition_used_for_attack.domain);

                if (is_attacking_weapon_disabled || is_out_of_range || is_not_of_the_same_domain)
                    continue;

                FireResult fire_result = FireResult(attacking_device, attacking_weapon_ptr, targeted_weapon);
                fire_result.starting_state = targeted_group.weapons_state.at(targeted_weapon_index_in_group);

                // Hit
                auto [target_was_hit, acc] = TryToHitTarget(ammunition_used_for_attack, distance_in_m);
                if (target_was_hit) {
                    fire_result.status = "HIT";
                    // Record damage
                    if (targeted_weapon->armor == Armor::Soft) {
                        auto dmg = ApplyModifiers(firing_side, params, ammunition_used_for_attack.soft);
                        fire_result.dmg = dmg;
                        targeted_weapon_state -= dmg;
                    } else {
                        auto dmg = ApplyModifiers(firing_side, params, ammunition_used_for_attack.soft);
                        targeted_weapon_state -= dmg;
                        fire_result.dmg = dmg;
                    }

                    if (targeted_weapon_state < 0)
                        targeted_weapon_state = 0;

                    fire_result.morale = targeted_group.morale.at(targeted_weapon_index_in_group);

                    // TODO: better morale
                    if (targeted_weapon_state <= 0) {
                        targeted_group.morale.at(targeted_weapon_index_in_group) = 0.0;
                    } else {
                        targeted_group.morale.at(targeted_weapon_index_in_group) -= 0.1;
                    }

                    targeted_group.weapons_state[targeted_weapon_index_in_group] = targeted_weapon_state;

                    fire_result.acc = acc;
                    fire_result.distance = distance_in_m;
                    fire_result.state_after_damage = attacking_weapon_state;
                    fire_result.morale_after_damage = targeted_group.morale.at(targeted_weapon_index_in_group);

                } else {
                    fire_result.status = "MISS";
                }

                results.push_back(fire_result);
            }
            attacking_weapon_index++;
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

std::string ArmorToStr(Armor armor)
{
    const static std::map<Armor, std::string> map = {

        { Armor::Hard, "Hard" },
        { Armor::Soft, "Soft" },
    };

    return map.at(armor);
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
