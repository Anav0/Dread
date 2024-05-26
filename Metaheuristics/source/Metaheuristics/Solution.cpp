#pragma once

#include "Solution.h"

void Solution::Copy(Solution& target)
{
    target.value = value;
    target.penealty = penealty;
    target.is_feasible = is_feasible;
    target.penealty_was_calculated = penealty_was_calculated;
    target.value_was_calculated = value_was_calculated;

    CopyTo(target);
}

void Solution::Reset()
{
    value = 0;
    penealty = 0;
    is_feasible = false;
    penealty_was_calculated = false;
    value_was_calculated = false;

    ResetTo();
}
