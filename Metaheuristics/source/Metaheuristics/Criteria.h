#pragma once

#include <vector>
#include <memory>

#include "Solution.h"

enum class OptimizationType {
    Min,
    Max
};

class Objective {
public:
    virtual f32 CalculateValue(Solution&) = 0;
};

class Constraint {
public:
    virtual f32 CalculatePenalty(Solution&) = 0;
};

class Criterion {

    std::vector<std::shared_ptr<Objective>> objectives;
    std::vector<std::shared_ptr<Constraint>> constraints;

public:
    OptimizationType type;

    Criterion(OptimizationType type, std::vector<std::shared_ptr<Objective>> objectives, std::vector<std::shared_ptr<Constraint>> constraints);
    void Init(Solution&);

    f32 CalculatePenalty(Solution&);
    f32 CalculateValue(Solution&);

    bool IsBetter(Solution& this_one, Solution& then_this);
};