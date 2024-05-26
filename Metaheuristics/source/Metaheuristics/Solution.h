#pragma once

#include "Misc/Constants.h"

class Solution {
protected:
    virtual void CopyTo(Solution& target) = 0;
    virtual void ResetTo() = 0;

public:
    f32 penealty = -1, value = -1;
    bool is_feasible, penealty_was_calculated, value_was_calculated;

    virtual std::unique_ptr<Solution> Clone() = 0;

    void Copy(Solution& target);
    void Reset();
};
