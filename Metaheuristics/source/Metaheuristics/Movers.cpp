#include "Movers.h"

void Mover::Move(Solution& sol) {
    sol.value = -1;
    sol.penealty = -1;

	sol.is_feasible = false;
    sol.penealty_was_calculated = false;
    sol.value_was_calculated = false;

    MoveEx(sol);
}
