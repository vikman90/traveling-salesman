/**
 * @file annealing_p.cpp
 * @brief Implementation of Parallel Simulated Annealing (PSA)
 * @author Vikman Fernandez-Castro
 * @date Created: March 3, 2013
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#include <vector>
#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

void parallelAnnealing(Cycle &data, int processes, int count, int migrLatency, unsigned int seed) {
    if (processes < 1) processes = 1;
    if (migrLatency < 1) migrLatency = 1;

    const int times = std::max(1, count / migrLatency);
    std::mt19937 generator(seed);
    Cycle &bestCycle = data;

    std::vector<Cycle> cycles(processes, data);
    for (int i = 0; i < processes; ++i) {
        cycles[i].shufflePath(generator);
    }

    for (int k = 1; k < times; ++k) {
        // Temperature cooling step across parallel threads/islands
        for (int i = 0; i < processes; ++i) {
            simulatedAnnealing(cycles[i], 1, generator, Swap);
        }

        // Identify best overall cycle
        for (int i = 0; i < processes; ++i) {
            if (cycles[i].getCost() < bestCycle.getCost()) {
                bestCycle.setPath(cycles[i]);
            }
        }

        // Migrate / synchronize best solution
        if (k < times - 1) {
            for (int i = 0; i < processes; ++i) {
                cycles[i].setPath(bestCycle);
            }
        }
    }
}

} // namespace Algorithms
