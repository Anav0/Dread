#include "StopCriteria.h"

bool NotGettingBetter::ShouldStop(Solution& sol)
{
    steps++;
    if (steps > max_steps)
        return true;

    auto is_better = type == OptimizationType::Max
        ? sol.value > best_sol_value
        : sol.value < best_sol_value;

    if (is_better) {
        best_sol_value = sol.value;
        best_sol_found_at_step = steps;
    }

    if (steps - best_sol_found_at_step > max_not_getting_better)
        return true;

    return false;
}

void NotGettingBetter::Reset()
{
    steps = 0;
    best_sol_found_at_step = 0;
    best_sol_value = type == OptimizationType::Max ? std::numeric_limits<f32>::max() : std::numeric_limits<f32>::min();
    cooler.Reset();
}

bool MaxSteps::ShouldStop(Solution& sol)
{
    steps++;
    if (steps > max_steps)
        return true;

    return false;
}

void MaxSteps::Reset()
{
    steps = 0;
}

