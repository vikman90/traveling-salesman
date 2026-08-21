/**
 * @file genetic.cpp
 * @brief Implementation of Genetic Algorithm (GA) for TSP
 * @author Vikman Fernandez-Castro
 * @date Created: March 1, 2013
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#include <iostream>
#include "../core/cycle.h"
#include "../core/population.h"
#include "algorithms.h"

namespace Algorithms {

void genetic(Cycle &data, int size, int count, Scheme scheme, unsigned int seed) {
    if (size < 3 || data.getSize() < 3) return;

    const int nMax = count * data.getSize();
    Cycle &bestCycle = data;
    std::mt19937 generator(seed);
    Population population(size, data, generator);

    data.setPath(population[population.bestCycle()]);

    const int step = (scheme == Generational ? size : 2);
    for (int i = 0; i < nMax; i += step) {
        population.evolve(scheme);
        int iBest = population.bestCycle();

        if (population[iBest].getCost() < bestCycle.getCost()) {
            bestCycle.setPath(population[iBest]);
        }
    }
}

} // namespace Algorithms
