#include "Criteria.h"

Criterion::Criterion(OptimizationType type, std::vector<std::shared_ptr<Objective>> objectives, std::vector<std::shared_ptr<Constraint>> constraints)
    : type(type)
    , objectives(objectives)
    , constraints(constraints)
{
}

void Criterion::Init(Solution& sol)
{
    CalculateValue(sol);

    if (!sol.is_feasible)
        CalculatePenalty(sol);
}

f32 Criterion::CalculatePenalty(Solution& sol)
{
    if (sol.penealty_was_calculated)
        return sol.penealty;

    f32 total = 0.0;
    for (auto& constraint : constraints) {
        total += constraint->CalculatePenalty(sol);
    }
    sol.penealty = total;
    sol.is_feasible = total == 0;
    sol.penealty_was_calculated = true;

    return total;
}

f32 Criterion::CalculateValue(Solution& sol)
{
    if (sol.value_was_calculated)
        return sol.value;

    f32 total = 0.0;
    for (auto& objective : objectives) {
        total += objective->CalculateValue(sol);
    }
    sol.value = total;
    sol.value_was_calculated = true;

    return total;
}

bool Criterion::IsBetter(Solution& first, Solution& second)
{
    if (!first.value_was_calculated)
        CalculateValue(first);

    if (!second.value_was_calculated)
        CalculateValue(second);

    if (first.is_feasible && !second.is_feasible)
        return true;

    if (first.is_feasible)
        return type == OptimizationType::Min ? CalculateValue(first) < CalculateValue(second) : CalculateValue(first) > CalculateValue(second);

    if (!second.is_feasible)
        return CalculatePenalty(first) < CalculatePenalty(second);

    return false;
}
