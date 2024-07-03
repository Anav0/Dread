#include <algorithm>
#include <format>
#include <random>

#include "Tests.h"

Armory Fixture_Armory()
{
    Ammo ap, he;

    ap.AA = 0.0;
    ap.accuracy.push_back({ 4000, 0.2 });
    ap.accuracy.push_back({ 2000, 0.6 });
    ap.domain = WeaponDomain::Ground;
    ap.hard = 100;
    ap.soft = 0;

    he.AA = 0.0;
    he.accuracy.push_back({ 4000, 0.2 });
    he.accuracy.push_back({ 2000, 0.6 });
    he.domain = WeaponDomain::Ground;
    he.hard = 0;
    he.soft = 100;

    Device cannon;
    cannon.name = "Test gun 1";
    cannon.ammunition.insert(0);
    cannon.ammunition.insert(1);

    WeaponSystem weapon1;
    weapon1.armor = Armor::Hard;
    weapon1.name = "Test Tank";
    weapon1.cost_in_dollars = 1000000;
    weapon1.devices.push_back(0);
    weapon1.domain = WeaponDomain::Ground;
    weapon1.default_state = 100;
    weapon1.type = WeaponSystemGeneralType::Tank;

    WeaponSystem weapon2 = WeaponSystem(weapon1);
    weapon2.name = "Test IFV";
    weapon2.cost_in_dollars = 1000000;
    weapon2.devices.push_back(0);
    weapon2.domain = WeaponDomain::Ground;
    weapon2.default_state = 100;
    weapon2.type = WeaponSystemGeneralType::IFV;

    WeaponSystem weapon3 = WeaponSystem(weapon2);
    weapon3.armor = Armor::Soft;
    weapon3.name = "Test Infantry";
    weapon3.cost_in_dollars = 1000000;
    weapon3.devices.push_back(0);
    weapon3.domain = WeaponDomain::Ground;
    weapon3.default_state = 100;
    weapon3.type = WeaponSystemGeneralType::Infantry;

    WeaponSystem weapon4 = WeaponSystem(weapon3);
    weapon4.armor = Armor::Soft;
    weapon4.name = "Test Drone";
    weapon4.cost_in_dollars = 1000000;
    weapon4.devices.push_back(0);
    weapon4.domain = WeaponDomain::Air;
    weapon4.default_state = 100;
    weapon4.type = WeaponSystemGeneralType::Drone;

    auto armory = Armory();
    armory.devices.push_back(cannon);

    armory.ammo.push_back(ap);
    armory.ammo.push_back(he);

    armory.weapons.push_back(weapon1);
    armory.weapons.push_back(weapon2);
    armory.weapons.push_back(weapon3);
    armory.weapons.push_back(weapon4);

    return armory;
}

std::vector<BattleGroup> Fixture_TestGroup(Armory* armory)
{
    const auto oblast = OblastCode::Kharkiv;
    const auto attacker = Side::UA;
    const auto modifiers_manager = ModifiersManager();
    const auto weather_manager = WeatherManager();

    std::vector<BattleGroup> test_groups;

    BattleGroup A = BattleGroup(armory);
    BattleGroup B = BattleGroup(armory);

    A.Attacking = 1;
    A.Domain = "Ground";
    A.name = "Test firing group";
    A.GroupIndex = 0;
    A.real_size = 1;
    A.parent_unit_index = 0;
    A.Side = "UA";
    A.weapons.at(0) = { nullptr, 0.2, 0.0, 1.0 };

    B = BattleGroup(A);
    B.weapons.at(0) = { nullptr, 0.4, 1.0, 1.0 };

    test_groups.push_back(A);
    test_groups.push_back(B);

    return test_groups;
}

void MoraleBroke_ReturnsTrueIfMoraleBelowThreshold()
{
    auto armory = Fixture_Armory();
    auto groups = Fixture_TestGroup(&armory);
    MoralBroke(groups, 0.7) ? PRINT_PASS : PRINT_FAILED;
}

void MoraleBroke_ReturnsFalseIfMoraleAboveThreshold()
{
    auto armory = Fixture_Armory();
    auto groups = Fixture_TestGroup(&armory);
    MoralBroke(groups, 0.2) ? PRINT_FAILED : PRINT_PASS;
}

void AverageDamageExceedsThreshold_ReturnsFalseIfDamageBelowThreshold()
{
    auto armory = Fixture_Armory();
    auto groups = Fixture_TestGroup(&armory);

    AverageDamageExceedsThreshold(groups, 0.9) ? PRINT_FAILED : PRINT_PASS;
}

void AverageDamageExceedsThreshold_ReturnsTrueIfDamageAboveThreshold()
{
    auto armory = Fixture_Armory();
    auto groups = Fixture_TestGroup(&armory);

    AverageDamageExceedsThreshold(groups, 0.2) ? PRINT_PASS : PRINT_FAILED;
}

void TryToHitTarget_CannotHitSomethingOutOfRange()
{
    Ammo ammo;
    ammo.accuracy.push_back({ 4500, 0.5 });
    std::random_device rd;
    auto rnd_engine = std::mt19937(rd());
    auto [hit, _] = TryToHitTarget(rnd_engine, &ammo, 6000);

    hit ? PRINT_FAILED : PRINT_PASS;
}

void TryToHitTarget_UsesCorrectAccuracyRating()
{
    std::random_device rd;
    auto rnd_engine = std::mt19937(rd());

    Ammo ammo;
    ammo.accuracy.push_back({ 4500, 0.5 });
    ammo.accuracy.push_back({ 3500, 0.6 });
    ammo.accuracy.push_back({ 2500, 0.7 });

    auto [hit1, acc1] = TryToHitTarget(rnd_engine, &ammo, 4000); // Should use 0.5 acc
    auto [hit2, acc2] = TryToHitTarget(rnd_engine, &ammo, 2500); // Should use 0.7 acc
    auto [hit3, acc3] = TryToHitTarget(rnd_engine, &ammo, 1000); // Should use 0.7 acc

    auto x = acc1 == 0.5;
    auto y = acc2 == 0.7;
    auto z = acc3 == 0.7;

    x&& y&& z ? PRINT_FAILED : PRINT_PASS;
}

void ApplyModifiers_GetsCorrectModifiers()
{
    WeatherManager weather_manager;

    ModifiersManager modifiers_manager;
    modifiers_manager.ru_modifier = { 0.5, 1.0 };
    modifiers_manager.ua_modifier = { 1.0, 0.5 };

    SimulationParams params = SimulationParams(OblastCode::Kharkiv, Side::UA, weather_manager, modifiers_manager);
    auto initial_dmg = 100;

    std::vector<bool> results;

    auto dmg = ApplyModifiers(Side::UA, params, initial_dmg); // UA fires in attack
    results.push_back(dmg == initial_dmg);

    dmg = ApplyModifiers(Side::RU, params, initial_dmg); // RU fires in defense
    results.push_back(dmg == initial_dmg);

    // Change of attacking side

    SimulationParams params2 = SimulationParams(OblastCode::Kharkiv, Side::RU, weather_manager, modifiers_manager);

    dmg = ApplyModifiers(Side::UA, params2, initial_dmg); // UA fires in defense
    results.push_back(dmg == 50);

    dmg = ApplyModifiers(Side::RU, params2, initial_dmg); // RU fires in attack
    results.push_back(dmg == 50);

    std::all_of(results.begin(), results.end(), [](bool v) { return v; }) ? PRINT_PASS : PRINT_FAILED;
}

void TryPickingRightAmmunitionForTarget_PickesAmmoThatCorrespondsToTargetArmorAndDomain()
{
    auto armory = Fixture_Armory();

    auto attacking_device = armory.devices.at(0);
    auto hard_target = armory.weapons.at(0);
    auto soft_target = armory.weapons.at(2);
    auto drone_target = armory.weapons.at(3);

    Ammo* ammo_used_for_hard_target = nullptr;
    Ammo* ammo_used_for_soft_target = nullptr;
    Ammo* ammo_used_for_diff_domain_target = nullptr;

    auto picked_for_hard = TryPickingRightAmmunitionForTarget(&armory, attacking_device, &hard_target, &ammo_used_for_hard_target);
    auto picked_for_soft = TryPickingRightAmmunitionForTarget(&armory, attacking_device, &soft_target, &ammo_used_for_soft_target);
    auto picked_for_diff_domain = TryPickingRightAmmunitionForTarget(&armory, attacking_device, &drone_target, &ammo_used_for_diff_domain_target);

    auto hard_passed = ammo_used_for_hard_target->hard > 0.0;
    auto soft_passed = ammo_used_for_soft_target->soft > 0.0;
    auto diff_domain_passed = ammo_used_for_diff_domain_target == nullptr && !picked_for_diff_domain;

    diff_domain_passed&& hard_passed&& soft_passed ? PRINT_PASS : PRINT_FAILED;
}

void RunTests()
{
    std::cout << "-----------------------\n";
    std::cout << "---- RUNNING TESTS ----\n";
    std::cout << "-----------------------\n";

    auto armory = Fixture_Armory();
    auto groups = Fixture_TestGroup(&armory);

    // Groups have avg morale of 0.3 by default
    MoraleBroke_ReturnsTrueIfMoraleBelowThreshold();
    MoraleBroke_ReturnsFalseIfMoraleAboveThreshold();

    // Groups have avg damage of 0.5 by default
    AverageDamageExceedsThreshold_ReturnsFalseIfDamageBelowThreshold();
    AverageDamageExceedsThreshold_ReturnsTrueIfDamageAboveThreshold();

    // Range
    TryToHitTarget_CannotHitSomethingOutOfRange();
    TryToHitTarget_UsesCorrectAccuracyRating();

    // Modifiers
    ApplyModifiers_GetsCorrectModifiers();

    // Ammo
    TryPickingRightAmmunitionForTarget_PickesAmmoThatCorrespondsToTargetArmorAndDomain();

    std::cout << "\n";
}
