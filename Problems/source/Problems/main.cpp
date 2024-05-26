#include "Metaheuristics/Annealing.h"

#include <random>

struct KnapsackInstance {
    u32 capacity;

    std::vector<u32> values;
    std::vector<u32> weights;

    KnapsackInstance(u32 capacity, std::vector<u32> values, std::vector<u32> weights)
        : capacity(capacity)
        , values(values)
        , weights(weights)
    {
    }
};

class KnapsackSolution : public Solution {
public:
    std::vector<u8> picked_items;

    KnapsackSolution(std::vector<u8> picked_items)
    {
        this->picked_items = picked_items;
    }

    KnapsackSolution(const KnapsackInstance& instance)
    {
        picked_items.reserve(instance.values.size());

        for (u32 i = 0; i < instance.values.size(); i++) {
            picked_items.push_back(1);
        }
    }

    // Inherited via Solution
    void CopyTo(Solution& target) override
    {
        KnapsackSolution& casted = dynamic_cast<KnapsackSolution&>(target);

        casted.picked_items.reserve(picked_items.size());

        for (u32 i = 0; i < casted.picked_items.size(); i++) {
            casted.picked_items.at(i) = picked_items.at(i);
        }

        casted.penealty = penealty;
        casted.value = value;
    }

    void ResetTo() override
    {
        for (u32 i = 0; i < picked_items.size(); i++) {
            picked_items[i] = 1;
        }
    }

    //TODO: move most of it to parent
    // Inherited via Solution
    std::unique_ptr<Solution> Clone() override
    {
        auto target = std::make_unique<KnapsackSolution>(picked_items);
        target->value = value;
        target->penealty = penealty;
        target->penealty_was_calculated = penealty_was_calculated;
        target->value_was_calculated = value_was_calculated;
        target->is_feasible = is_feasible;

        return target;
    }
};

class KnapsackObj : public Objective {
public:
    const KnapsackInstance& instance;

    KnapsackObj(const KnapsackInstance& instance)
        : instance(instance)
    {
    }

    // Inherited via Objective
    f32 CalculateValue(Solution& target) override
    {
        KnapsackSolution& casted = dynamic_cast<KnapsackSolution&>(target);

        f32 total = 0.0;

        for (u32 i = 0; i < casted.picked_items.size(); i++) {
            if (!casted.picked_items.at(i))
                continue;

            total += this->instance.values.at(i);
        }

        return total;
    }
};

class KnapsackMover : public Mover {
private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dist;

public:
    KnapsackMover(const KnapsackInstance& instance)
    {
        gen = std::mt19937(rd());
        dist = std::uniform_int_distribution<>(0, instance.values.size());
    }

    // Inherited via Mover
    void MoveEx(Solution& target) override
    {
        KnapsackSolution& casted = dynamic_cast<KnapsackSolution&>(target);

        auto random_index = dist(gen);
        casted.picked_items[random_index] = casted.picked_items[random_index] ^ 1;
    }

    void Reset() override
    {
    }
};

class KnapsackConstraint : public Constraint {

public:
    const KnapsackInstance& instance;

    KnapsackConstraint(const KnapsackInstance& instance)
        : instance(instance)
    {
    }

    // Inherited via Constraint
    f32 CalculatePenalty(Solution& target) override
    {
        KnapsackSolution& casted = dynamic_cast<KnapsackSolution&>(target);

        f32 total = 0.0;

        for (u32 i = 0; i < casted.picked_items.size(); i++) {
            if (!casted.picked_items.at(i))
                continue;

            total += this->instance.weights.at(i);
        }

        return total - instance.capacity;
    }
};

int main()
{
    KnapsackInstance instance(6, { 2, 2, 2, 10 }, { 1, 2, 3, 4 });
    KnapsackSolution solution(instance);
    KnapsackMover mover(instance);

    QuadriaticCooler cooler = QuadriaticCooler(1000, 0.997);
    NotGettingBetter criteria = NotGettingBetter(cooler, 50000, 5000, OptimizationType::Max);

    Annealing annealing(mover, cooler, criteria);

    std::vector<std::shared_ptr<Objective>> objectives;
    std::vector<std::shared_ptr<Constraint>> constraints;

    constraints.push_back(std::make_unique<KnapsackConstraint>(instance));
    objectives.push_back(std::make_unique<KnapsackObj>(instance));

    Criterion criterion(OptimizationType::Max, objectives, constraints);

    annealing.Solve(solution, criterion);

    printf("Is feasible: %i, Value: %f, Penealty: %f\n", solution.is_feasible, solution.value, solution.penealty);
}