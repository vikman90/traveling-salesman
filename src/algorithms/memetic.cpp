/**
 * @file memetic.cpp
 * @brief Implementation of Memetic Algorithm (GA + 2-Opt Local Search Hybridization)
 */

#include "../core/cycle.h"
#include "../core/population.h"
#include "algorithms.h"

namespace Algorithms {

void memetic(Cycle &data, int size, int count, Hybridization hybridization, unsigned int seed) {
    if (size < 3 || data.getSize() < 3) return;

    const int nMax = count * data.getSize();
    int nEvolves = 0;
    int best1 = 0, best2 = 0;
    Cycle &bestCycle = data;
    std::mt19937 generator(seed);
    Population population(size, data, generator);

    data.setPath(population[population.bestCycle()]);

    for (int i = 0; i < nMax; i += size) {
        population.evolve(Generational);
        nEvolves++;

        switch (hybridization) {
        case Everygen_Twochrom:
            population.bestCycles(best1, best2);
            i += localSearch(population[best1]);
            i += localSearch(population[best2]);
            break;

        case Everygen_Everychrom:
            for (int j = 0; j < size; ++j) {
                i += localSearch(population[j]);
            }
            break;

        case Tengen_Twochrom:
            if (nEvolves % 10 == 0) {
                population.bestCycles(best1, best2);
                i += localSearch(population[best1]);
                i += localSearch(population[best2]);
            }
            break;

        case Tengen_Everychrom:
            if (nEvolves % 10 == 0) {
                for (int j = 0; j < size; ++j) {
                    i += localSearch(population[j]);
                }
            }
            break;
        }

        int iBest = population.bestCycle();
        if (population[iBest].getCost() < bestCycle.getCost()) {
            bestCycle.setPath(population[iBest]);
        }
    }
}

} // namespace Algorithms
