#include "Annealing.h"

#include <cassert>

bool Annealing::HaveEnoughEnergy(Criterion& criterion, Solution& after_move, Solution& before_move)
{
    auto rnd = dist(gen);

    f32 after_move_value = 0.0;
    f32 before_move_value = 0.0;

    if (after_move.is_feasible) {
        after_move_value = after_move.value;
        before_move_value = before_move.value;
    } else {
        after_move_value = after_move.penealty;
        before_move_value = before_move.penealty;
    }

    auto diff
        = criterion.type == OptimizationType::Min ? before_move_value - after_move_value : after_move_value - before_move_value;

    if (diff == 0)
        return false;

    if (diff > 0)
        return true;

    auto energy = exp(diff / cooler.temperature);

    return rnd < energy;
}

void Annealing::Solve(Solution& solution, Criterion& criterion)
{
    this->iter = 0;

    this->Reset();
    solution.Reset();

    criterion.Init(solution);

    auto best_sol = solution.Clone();
    auto sol_before_move = solution.Clone();

    while (!this->stop_criteria.ShouldStop(solution)) {

        if (iter % 100 == 0) {
            printf("%i\r", iter);
        }

        solution.Copy(*sol_before_move);

        this->mover.Move(solution);

        if (criterion.IsBetter(solution, *sol_before_move) || HaveEnoughEnergy(criterion, solution, *sol_before_move)) {
            if (criterion.IsBetter(solution, *best_sol))
                solution.Copy(*best_sol);
        } else {
            sol_before_move->Copy(solution);
        }

        iter++;
    }

    best_sol->Copy(solution);
}

void Annealing::Reset()
{
    cooler.Reset();
    stop_criteria.Reset();
    mover.Reset();
}
