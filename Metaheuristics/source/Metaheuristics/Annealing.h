#pragma once

#include <random>
#include <type_traits>

#include "Misc/Constants.h"

#include "Cooler.h"
#include "Criteria.h"
#include "Movers.h"
#include "Solution.h"
#include "StopCriteria.h"

class Annealing {
private:
    Mover& mover;
    Cooler& cooler;
    StopCriteria& stop_criteria;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dist;

public:
    u32 iter;

    bool HaveEnoughEnergy(Criterion&, Solution& after_move, Solution& before_move);
    void Solve(Solution& solution, Criterion& criterion);
    void Reset();

    Annealing(Mover& mover, Cooler& cooler, StopCriteria& stop_criteria)
        : mover(mover)
        , cooler(cooler)
        , stop_criteria(stop_criteria)
    {
        gen = std::mt19937(rd());
        dist = std::uniform_real_distribution<>();
    }
};
