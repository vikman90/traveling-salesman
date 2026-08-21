/**
 * @file population.h
 * @brief Declaration of the Population class for evolutionary TSP algorithms
 */

#ifndef TSP_CORE_POPULATION_H
#define TSP_CORE_POPULATION_H

#include <vector>
#include <random>
#include "cycle.h"
#include "../algorithms/algorithms.h"

/**
 * @brief Population Class
 *
 * Represents an ensemble of TSP chromosome candidate solutions evolving through
 * crossover, mutation, and selection schemes.
 */
class Population {
public:
    Population() = default;
    explicit Population(int size, const Cycle &sample, std::mt19937 &generator);

    // Rule of Zero
    Population(const Population &) = default;
    Population(Population &&) noexcept = default;
    ~Population() = default;
    Population &operator=(const Population &) = default;
    Population &operator=(Population &&) noexcept = default;

    inline Cycle &operator[](int index) {
        return chromosomes[index];
    }

    inline const Cycle &operator[](int index) const {
        return chromosomes[index];
    }

    [[nodiscard]] inline int getSize() const noexcept {
        return static_cast<int>(chromosomes.size());
    }

    [[nodiscard]] int bestCycle() const;
    void bestCycles(int &iFirst, int &iSecond) const;
    [[nodiscard]] int worstCycle() const;
    [[nodiscard]] float meanCost() const;

    void evolve(Algorithms::Scheme scheme);

private:
    int size{0};
    std::vector<Cycle> chromosomes;
    std::mt19937 *generator{nullptr};

    int select(int iLast) const;
    Cycle cross(int iFather, int iMother);
    void replace(int iFather, int iMother, Cycle &son, Cycle &daughter, int iLast);
    void replace(Cycle &son, Cycle &daughter);

    bool mutateNow();
    bool crossNow();
};

#endif // TSP_CORE_POPULATION_H
