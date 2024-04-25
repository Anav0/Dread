#include "Fight.h"
#include "GameState.h"

#include <algorithm>
#include <numeric>
#include <random>

std::vector<BattleGroup> Fight::FormRUGroup(Armory* armory, UnitStance stance, Deployment& deployment)
{
    std::vector<BattleGroup> battle_groups;
    for (u8 i = 0; i < MAX_UNITS; i++) {
        UnitStance ru_unit_stance = this->ru_stance[i];
        if (ru_unit_stance == stance) {
            i32 ru_unit_index = this->ru_units[i];
            if (ru_unit_index != -1) {
                Unit& unit = deployment.ru_units.at(ru_unit_index);
                battle_groups.push_back(FormBattleGroup(armory, ru_unit_index, unit));
            }
        }
    }
    return battle_groups;
}

std::vector<BattleGroup> Fight::FormUAGroup(Armory* armory, UnitStance stance, Deployment& deployment)
{
    std::vector<BattleGroup> battle_groups;
    bool pushed_something_to_group = false;
    for (u8 i = 0; i < MAX_UNITS; i++) {
        UnitStance ua_unit_stance = this->ua_stance[i];
        if (ua_unit_stance == stance) {
            i32 ua_unit_index = this->ua_units[i];
            if (ua_unit_index != -1) {
                Unit& unit = deployment.ukr_units.at(ua_unit_index);
                battle_groups.push_back(FormBattleGroup(armory, ua_unit_index, unit));
                pushed_something_to_group = true;
            }
        }
    }
    assert(pushed_something_to_group);
    return battle_groups;
}

void SaveGroups(SimulationSession& sim_session, std::vector<BattleGroup>& groups)
{
    for (auto& group : groups) {
        group.round_info.run = sim_session.run;
        group.round_info.round = sim_session.round;
        group.round_info.distance = sim_session.distance_in_meters;
        sim_session.battle_groups_saver.AddRow(group);
    }
}

void Fight::SimulateAttack(Armory* armory, Deployment& deployment, SimulationSession& simulation_session)
{
    // TODO: initiative system for determining defenders and attackers
    Side attacking_side = Side::UA;
    Side defending_side = Side::RU;

    std::vector<BattleGroup> attacker_battle_grup = FormRUGroup(armory, UnitStance::Committed, deployment);
    std::vector<BattleGroup> defender_battle_grup = FormUAGroup(armory, UnitStance::Defending, deployment);

    u32 group_index = 0;
    for (auto& group : attacker_battle_grup) {
        group.Attacking = 1;
        group.Side = "RU";
        group.Domain = "Ground";
        group.GroupIndex = group_index;
        group_index++;
    }

    group_index = 0;
    for (auto& group : defender_battle_grup) {
        group.Attacking = 0;
        group.Side = "UA";
        group.Domain = "Ground";
        group.GroupIndex = group_index;
        group_index++;
    }

    u32 round = 0;

    bool attacker_moral_broke = false;
    bool defender_moral_broke = false;
    bool attacker_was_mauled = false;
    bool defender_was_mauled = false;
    f32 damage_threshold = 0.5;

    while (!defender_moral_broke && !attacker_moral_broke && !attacker_was_mauled && !defender_was_mauled) {

        defender_moral_broke = MoralBroke(defender_battle_grup);
        attacker_moral_broke = MoralBroke(attacker_battle_grup);

        attacker_was_mauled = AverageDamageExceedsThreshold(attacker_battle_grup, damage_threshold);
        defender_was_mauled = AverageDamageExceedsThreshold(defender_battle_grup, damage_threshold);

        simulation_session.round = round;

        // Attacking groups go forward!
        if (this->attacker_distance_in_meters > 0) {
            this->attacker_distance_in_meters -= 100;
            if (this->attacker_distance_in_meters < 0)
                this->attacker_distance_in_meters = 0;
        }

        simulation_session.distance_in_meters = this->attacker_distance_in_meters;

        auto rounds = Fire(armory, this->attacker_distance_in_meters, defender_battle_grup, attacker_battle_grup);

        if (simulation_session.ShouldSaveFiring()) {
            simulation_session.AddRound(rounds, "UA");
        }

        rounds = Fire(armory, this->attacker_distance_in_meters, attacker_battle_grup, defender_battle_grup);

        if (simulation_session.ShouldSaveFiring()) {
            simulation_session.AddRound(rounds, "RU");
        }

        if (simulation_session.ShouldSaveGroups()) {
            SaveGroups(simulation_session, attacker_battle_grup);
            SaveGroups(simulation_session, defender_battle_grup);
        }

        round++;

        assert(round < 10000);
    }

    bool defender_won = attacker_moral_broke || attacker_was_mauled;

    if (defender_won) {
        // assert(!defender_moral_broke && !defender_was_mauled);
    }

    if (!defender_won) {
        // assert(!attacker_moral_broke && !attacker_was_mauled);
    }

    if (simulation_session.ShouldSaveResult()) {
        simulation_session.AddWinner(defender_won ? defending_side : attacking_side);
    }
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

bool MoralBroke(std::vector<BattleGroup>& groups)
{
    f32 morale_threshold = 0.5;
    for (BattleGroup& group : groups) {
        f32 avg_morale = std::reduce(group.morale.begin(), group.morale.end()) / group.morale.size();
        if (avg_morale < morale_threshold)
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

std::vector<FireResult> Fire(Armory* armory, u32 distance_in_m, std::vector<BattleGroup>& fire, std::vector<BattleGroup>& target)
{
    std::vector<FireResult> results;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<u32> dist(0, target.size() - 1);
    std::uniform_int_distribution<u32> d6(0, 6);

    for (BattleGroup& group : fire) {
        u32 weapon_index = 0;
        for (WeaponSystem* weapon : group.weapons) {
            for (u32 device_index : weapon->devices) {
                auto& device = armory->devices.at(device_index);

                auto [firing_target_index, firing_target, target_group] = PickTarget(dist, mt, target);
                auto& ammunition = PickRightAmmunitionForTarget(armory, device, firing_target);

                // TODO: temporary
                f32* firing_target_state = &group.weapons_state.at(weapon_index);
                if (*firing_target_state <= 0 || (firing_target->domain != ammunition.domain))
                    continue;

                if (ammunition.accuracy[0].range_in_meters < distance_in_m) {
                    continue;
                }

                FireResult fire_result = FireResult(device, weapon, firing_target);

                fire_result.starting_state = *firing_target_state;

                // Hit
                auto [was_hit, acc] = TryToHitTarget(ammunition, distance_in_m);
                if (was_hit && *firing_target_state > 0.0) {
                    fire_result.status = "HIT";
                    // Record damage
                    if (firing_target->armor == Armor::Soft) {
                        *firing_target_state -= ammunition.soft;
                        fire_result.dmg = ammunition.soft;

                    } else {
                        *firing_target_state -= ammunition.hard;
                        fire_result.dmg = ammunition.hard;
                    }

                    if (*firing_target_state < 0)
                        *firing_target_state = 0;

                    fire_result.morale = target_group.morale.at(firing_target_index);

                    // TODO: better morale
                    if (*firing_target_state <= 0) {
                        target_group.morale.at(firing_target_index) = 0.0;
                    } else {
                        target_group.morale.at(firing_target_index) -= 0.1;
                    }

                    fire_result.acc = acc;
                    fire_result.distance = distance_in_m;
                    fire_result.state_after_damage = *firing_target_state;
                    fire_result.morale_after_damage = target_group.morale.at(firing_target_index);

                } else {
                    fire_result.status = "MISS";
                }

                results.push_back(fire_result);
            }
            weapon_index++;
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
