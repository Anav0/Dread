#pragma once

#include "Metaheuristics/Annealing.h"
#include "Misc/Constants.h"

#include <random>
#include <vector>

struct DreadInstance {
    Fight fight;
    SimulationParams params;
    Deployment deployment;

    DreadInstance(Fight fight, SimulationParams params, Deployment deployment)
        : fight(fight)
        , params(params)
        , deployment(deployment)
    {
    }
};

class DreadSolution : public Solution {
public:
    Armory armory;
    Armory starting_armory;

    // Inherited via Solution
    void CopyTo(Solution& target) override
    {
        DreadSolution& casted = dynamic_cast<DreadSolution&>(target);
        memcpy(this, &casted, sizeof(DreadSolution));
    }

    void ResetTo() override
    {
        //TODO: check
        armory = starting_armory;
    }

    // TODO: move most of it to parent
    // Inherited via Solution
    std::unique_ptr<Solution> Clone() override
    {
        auto target = std::make_unique<DreadSolution>();

        return target;
    }
};

struct ExpectedLosses {
    u32 index_in_armory;
    f32 lost_percent;
};

class DreadObj : public Objective {
public:
    DreadInstance& instance;

    std::vector<ExpectedLosses> losses;

    u32 runs;

    DreadObj(u32 runs, DreadInstance& instance, std::vector<ExpectedLosses> losses)
        : instance(instance)
        , runs(runs)
        , losses(losses)
    {
    }

    // Inherited via Objective
    f32 CalculateValue(Solution& target) override
    {
        DreadSolution& casted = dynamic_cast<DreadSolution&>(target);

        u32 victories = 0.0;

        for (u32 i = 0; i < runs; i++) {
            auto result = instance.fight.SimulateAttack(instance.params, &casted.armory, instance.deployment, nullptr);
        }

        return static_cast<f32>(runs) / victories;
    }
};

class DreadMover : public Mover {
private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dist;

public:
    DreadMover(const DreadInstance& instance)
    {
        gen = std::mt19937(rd());
        // dist = std::uniform_int_distribution<>(0, instance.values.size());
    }

    // Inherited via Mover
    void MoveEx(Solution& target) override
    {
        DreadSolution& casted = dynamic_cast<DreadSolution&>(target);

        auto random_index = dist(gen);
    }

    void Reset() override
    {
    }
};

class DreadConstraint : public Constraint {

public:
    const DreadInstance& instance;

    DreadConstraint(const DreadInstance& instance)
        : instance(instance)
    {
    }

    // Inherited via Constraint
    f32 CalculatePenalty(Solution& target) override
    {
        DreadSolution& casted = dynamic_cast<DreadSolution&>(target);

        f32 total = 0.0;

        return total;
    }
};
