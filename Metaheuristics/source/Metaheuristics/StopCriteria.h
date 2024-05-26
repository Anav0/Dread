#pragma once

#include "Cooler.h"
#include "Criteria.h"
#include "Solution.h"

class StopCriteria {
public:
    virtual bool ShouldStop(Solution&) = 0;
    virtual void Reset() = 0;
};

class NotGettingBetter : public StopCriteria {
private:
    Cooler& cooler;
    u32 steps = 0;
    u32 max_steps;
    u32 max_not_getting_better;
    OptimizationType type;

    u32 best_sol_found_at_step;
    f32 best_sol_value;

public:
    // Inherited via StopCriteria
    bool ShouldStop(Solution&) override;
    void Reset() override;

    NotGettingBetter(Cooler& cooler, u32 max_steps, u32 max_not_getting_better, OptimizationType type)
        : cooler(cooler)
        , max_steps(max_steps)
        , max_not_getting_better(max_not_getting_better)
        , type(type)
    {
        Reset();
    }
};

class MaxSteps : public StopCriteria {
private:
    u32 steps = 0;
    u32 max_steps;

public:
    // Inherited via StopCriteria
    bool ShouldStop(Solution&) override;
    void Reset() override;

    MaxSteps(u32 max_steps) : max_steps(max_steps)
    {
        Reset();
    }
};
