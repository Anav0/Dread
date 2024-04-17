#include "Fight.h"
#include "GameState.h"

#include <algorithm>
#include <numeric>
#include <random>

static BattleGroup FormBattleGroup(Armory& armory, u32 parent_unit_index, Unit& unit);

std::vector<BattleGroup> Fight::FormRUGroup(Armory& armory, UnitStance stance, Deployment& deployment)
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

std::vector<BattleGroup> Fight::FormUAGroup(Armory& armory, UnitStance stance, Deployment& deployment)
{
    std::vector<BattleGroup> battle_groups;
    for (u8 i = 0; i < MAX_UNITS; i++) {
        UnitStance ua_unit_stance = this->ua_stance[i];
        if (ua_unit_stance == stance) {
            i32 ua_unit_index = this->ua_units[i];
            if (ua_unit_index != -1) {
                Unit& unit = deployment.ukr_units.at(ua_unit_index);
                battle_groups.push_back(FormBattleGroup(armory, ua_unit_index, unit));
            }
        }
    }
    return battle_groups;
}

void Fight::SimulateAttack(Armory& armory, Deployment& deployment)
{
    // TODO: initiative system for determining defenders and attackers
    Side attacking_side = Side::UA;
    Side defending_side = Side::RU;

    std::vector<BattleGroup> attacker_battle_grup = FormRUGroup(armory, UnitStance::Committed, deployment);
    std::vector<BattleGroup> defender_battle_grup = FormUAGroup(armory, UnitStance::Defending, deployment);

    if (saver != nullptr) {
        saver->Open(std::format("data.csv").c_str());
        saver->AddHeader("Iter;Side;Status;Weapon;Device;ACC;TargetWeapon;StartingState;Dmg;StateAfterHit;Morale;MoraleAfterHit;Distance");
    }

    SimulationSession sim_session = SimulationSession(armory, deployment);
    sim_session.iter = 0;
    sim_session.rounds.reserve(20);

    u32 iter = 0;
    while (!MoralBroke(defender_battle_grup) && !MoralBroke(attacker_battle_grup) && !AverageDamageExceedsThreshold(attacker_battle_grup, 0.5) && !AverageDamageExceedsThreshold(defender_battle_grup, 0.5)) {

        // Attacking groups go forward!
        if (this->attacker_distance_in_meters > 0) {
            this->attacker_distance_in_meters -= 100;
            if (this->attacker_distance_in_meters < 0)
                this->attacker_distance_in_meters = 0;
        }

        auto rounds = Fire(armory, this->attacker_distance_in_meters, defender_battle_grup, attacker_battle_grup);

        if (saver != nullptr) {
            for (FireResult& r : rounds) {
                r.firing_side = "UA";
                r.iter = iter;
                saver->AddRow(r);
            }
        }

        rounds = Fire(armory, this->attacker_distance_in_meters, attacker_battle_grup, defender_battle_grup);

        if (saver != nullptr) {
            for (FireResult& r : rounds) {
                r.firing_side = "RU";
                r.iter = iter;
                saver->AddRow(r);
            }
        }

        this->phase++;
        printf("Phase: %i\n", this->phase);
        printf("Distance: %im\n", this->attacker_distance_in_meters);

        // # Iter Status Weapon Device ACC TargetWeapon StartingState Dmg StateAfterHit Distance
        // 0    HIT   BMP2   2A42   0.5 BMP1 100 24 76 2400

        iter++;
    }

    if (saver != nullptr) {
        saver->Flush();
        saver->Close();
    }
}

static BattleGroup FormBattleGroup(Armory& armory, u32 parent_unit_index, Unit& unit)
{
    BattleGroup group;

    group.name = "BG from " + unit.name;
    group.parent_unit_index = parent_unit_index;

    u32 index = 0;
    for (u32 weapon_index : unit.weapons) {
        WeaponSystem* weapon_ref = &armory.weapons[weapon_index];
        group.weapons.push_back(weapon_ref);
        u32 n = unit.weapons_counter[index] * 0.3;
        unit.weapons_counter[index] -= n;
        group.weapons_counter.push_back(n);
        group.morale.push_back(unit.morale[index]);
        index++;
    }

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
        for (auto& weapon_system : group.weapons) {
            if (weapon_system->state <= 0) {
                destroyed++;
            } else if (weapon_system->state < weapon_system->inital_state) {
                damaged++;
            } else {
                intact++;
            }
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

static Ammo& PickRightAmmunitionForTarget(Armory& armory, Device& firing_device, WeaponSystem* target)
{
    assert(target != nullptr);
    return armory.ammo.at(*firing_device.ammunition.begin());
}

std::vector<FireResult> Fire(Armory& armory, u32 distance_in_m, std::vector<BattleGroup>& fire, std::vector<BattleGroup>& target)
{
    std::vector<FireResult> results;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<u32> dist(0, target.size() - 1);
    std::uniform_int_distribution<u32> d6(0, 6);

    for (BattleGroup& group : fire) {
        for (WeaponSystem* weapon : group.weapons) {
            for (u32 device_index : weapon->devices) {
                auto& device = armory.devices.at(device_index);

                auto [firing_target_index, firing_target, target_group] = PickTarget(dist, mt, target);
                auto& ammunition = PickRightAmmunitionForTarget(armory, device, firing_target);

                //TODO: temporary
                if (firing_target->state <= 0 || (firing_target->domain != ammunition.domain))
                    continue;

                if (ammunition.accuracy[0].range_in_meters < distance_in_m) {
                    printf("No ammo able to reach target at %im\n", distance_in_m);
                    continue;
                }

                printf("%s from %s fires at: %s ", device.name.c_str(), group.name.c_str(), firing_target->name.c_str());

                FireResult fire_result = FireResult(device, weapon, firing_target);

                fire_result.starting_state = firing_target->state;

                // Hit
                auto [was_hit, acc] = TryToHitTarget(ammunition, distance_in_m);
                if (was_hit && firing_target->state > 0.0) {
                    printf("[HIT] ");
                    fire_result.status = "HIT";
                    // Record damage
                    if (firing_target->armor == Armor::Soft) {
                        printf("SOFT: %f -= %i! ", firing_target->state, ammunition.soft);
                        firing_target->state -= ammunition.soft;
                        fire_result.dmg = ammunition.soft;

                    } else {
                        printf("HARD: %f -= %i! ", firing_target->state, ammunition.hard);
                        firing_target->state -= ammunition.hard;
                        fire_result.dmg = ammunition.hard;
                    }

                    fire_result.morale = target_group.morale.at(firing_target_index);

                    //TODO: better morale
                    if (firing_target->state <= 0) {
                        target_group.morale.at(firing_target_index) = 0.0;
                    } else {
                        target_group.morale.at(firing_target_index) -= 0.1;
                    }

                    fire_result.acc = acc;
                    fire_result.distance = distance_in_m;
                    fire_result.state_after_damage = firing_target->state;
                    fire_result.morale_after_damage = target_group.morale.at(firing_target_index);

                    printf("State: %f, Morale: %f\n", firing_target->state, target_group.morale.at(firing_target_index));
                } else {
                    printf("[MISS]\n");
                    fire_result.status = "MISS";
                }

                results.push_back(fire_result);
            }
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
