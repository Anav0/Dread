#include <Game/Devices.h>
#include <Game/Fight.h>
#include <Game/Modifiers.h>
#include <Game/Weather.h>

#include "Misc/CsvSaver.h"

#include "Problem.h"

#include <format>
#include <random>

const char* units_path = "D:/Projects/Dread/Game/data/units.csv";
const char* armory_path = "D:/Projects/Dread/Game/data/weapons.csv";
const char* storage_path = "D:/Projects/Dread/Game/data/storage.csv";
const char* conditions_path = "D:/Projects/Dread/Game/data/conditions.csv";

static const BiMap<OblastCode, const std::string> OBLASTS = {
    { OblastCode::Zythomyr, "Zythomyr" },
    { OblastCode::Zaporizhia, "Zaporizhia" },
    { OblastCode::Zakarpatia, "Zakarpatia" },
    { OblastCode::Volyn, "Volyn" },
    { OblastCode::Vinnytsia, "Vinnytsia" },
    { OblastCode::Ternopil, "Ternopil" },
    { OblastCode::Sumy, "Sumy" },
    { OblastCode::Rivne, "Rivne" },
    { OblastCode::Poltava, "Poltava" },
    { OblastCode::Odessa, "Odessa" },
    { OblastCode::Mykolaiv, "Mykolaiv" },
    { OblastCode::Lviv, "Lviv" },
    { OblastCode::Luhansk, "Luhansk" },
    { OblastCode::Kirovohrad, "Kirovohrad" },
    { OblastCode::Kiev, "Kiev" },
    { OblastCode::Chmielnicki, "Chmielnicki" },
    { OblastCode::Kherson, "Kherson" },
    { OblastCode::Kharkiv, "Kharkiv" },
    { OblastCode::IvanoFrankivsk, "IvanoFrankivsk" },
    { OblastCode::Donetsk, "Donetsk" },
    { OblastCode::Dnipropetrovsk, "Dnipropetrovsk" },
    { OblastCode::Crimea, "Crimea" },
    { OblastCode::Chernihiv, "Chernihiv" },
    { OblastCode::Chernivtsi, "Chernivtsi" },
    { OblastCode::Cherkasy, "Cherkasy" },
};

constexpr u32 MAX_RUNS = 50;

// TODO: copy pasta from Devices.cpp
std::vector<std::string> split2(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::map<OblastCode, std::tuple<Weather, GroundCondition>> GetInitialConditions2()
{
    std::map<OblastCode, std::tuple<Weather, GroundCondition>> conditions;
    for (auto it = INITIAL_CONTROL.begin(); it != INITIAL_CONTROL.end(); ++it) {
        conditions.insert({ it->first, { Weather::Clear, GroundCondition::Dry } });
    }
    return conditions;
}

#define PRINT_PASS std::cout << std::format("{:70} {:->1}\n", __func__, " PASS")
#define PRINT_FAILED std::cout << std::format("{:70} {:->1}", __func__, " FAIL")

static Armory Fixture_Armory()
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

    auto armory = Armory();
    armory.devices.push_back(cannon);
    armory.ammo.push_back(ap);
    armory.ammo.push_back(he);

    return armory;
}

static std::vector<BattleGroup> Fixture_TestGroup(Armory* armory)
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

static void MoraleBroke_ReturnsTrueIfMoraleBelowThreshold()
{
    auto armory = Fixture_Armory();
    auto groups = Fixture_TestGroup(&armory);
    MoralBroke(groups, 0.7) ? PRINT_PASS : PRINT_FAILED;
}

static void MoraleBroke_ReturnsFalseIfMoraleAboveThreshold()
{
    auto armory = Fixture_Armory();
    auto groups = Fixture_TestGroup(&armory);
    MoralBroke(groups, 0.2) ? PRINT_FAILED : PRINT_PASS;
}

static void AverageDamageExceedsThreshold_ReturnsFalseIfDamageBelowThreshold()
{
    auto armory = Fixture_Armory();
    auto groups = Fixture_TestGroup(&armory);

    AverageDamageExceedsThreshold(groups, 0.9) ? PRINT_FAILED : PRINT_PASS;
}

static void AverageDamageExceedsThreshold_ReturnsTrueIfDamageAboveThreshold()
{
    auto armory = Fixture_Armory();
    auto groups = Fixture_TestGroup(&armory);

    AverageDamageExceedsThreshold(groups, 0.2) ? PRINT_PASS : PRINT_FAILED;
}

static void TryToHitTarget_CannotHitSomethingOutOfRange()
{
    Ammo ammo;
    ammo.accuracy.push_back({ 4500, 0.5 });
    std::random_device rd;
    auto rnd_engine = std::mt19937(rd());
    auto [hit, _] = TryToHitTarget(rnd_engine, &ammo, 6000);

    hit ? PRINT_FAILED : PRINT_PASS;
}

static void RunTests()
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

    std::cout << "\n";
}

int main(int argc, char* argv[])
{
    RunTests();

    assert(argc == 2);

    auto save_only_this_arr = split2(argv[1], ';');
    auto save_only_this = std::set<std::string>(save_only_this_arr.begin(), save_only_this_arr.end());

    Armory armory = LoadArmory(armory_path, storage_path);
    Deployment deployment = LoadUnits(armory.weapons, OBLASTS, units_path);

    SimulationSession session = SimulationSession(&armory, deployment, save_only_this);

    WeatherManager weather_manager = WeatherManager();
    weather_manager.Init(GetInitialConditions2());
    ModifiersManager modifiers_manager = ModifiersManager();
    modifiers_manager.LoadWeatherModifiers(conditions_path);
    modifiers_manager.ru_modifier = Modifier(0.55, 1.0);
    modifiers_manager.ua_modifier = Modifier(1.1, 1.25);

    SimulationParams params = SimulationParams(OblastCode::Donetsk, Side::RU, weather_manager, modifiers_manager);

    Fight fight;

    for (u32 run = 0; run < MAX_RUNS; run++) {
        if (true) {
            printf("%i/%i\r", run, MAX_RUNS);
        }

        fight.attacker_distance_in_meters = 6000;

        fight.ua_units[0] = 0;
        fight.ua_stance[0] = UnitStance::Defending;

        fight.ru_units[0] = 0;
        fight.ru_stance[0] = UnitStance::Committed;

        session.run = run;

        Armory armory_cpy = Armory(armory);
        Deployment deployment_cpy = Deployment(deployment);

        session.armory = &armory_cpy;
        session.deployment = deployment_cpy;

        assert(armory_cpy.weapons.size() == armory.weapons.size());

        fight.SimulateAttack(params, &armory_cpy, deployment_cpy, &session);
    }

    printf("Flush!\n");
    session.Flush();
    printf("Close!\n");
    session.Close();

    return 0;
}