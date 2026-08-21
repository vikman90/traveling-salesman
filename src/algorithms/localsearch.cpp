/**
 * @file localsearch.cpp
 * @brief Implementation of 2-Opt Local Search (First Improvement)
 * @author Vikman Fernandez-Castro
 * @date Created: October 24, 2012
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

int localSearch(Cycle &data) {
    const int n = data.getSize();
    if (n < 3) return 0;

    const int evalsPerLoop = (n * (n - 1)) / 2;
    int evals = 0;
    bool updated = false;
    Cycle curCycle = data;
    Cycle bestCycle = data;

    do {
        curCycle.setPath(bestCycle);
        updated = false;

        for (int i = 1; i < n; ++i) {
            for (int j = 0; j < i; ++j) {
                curCycle.swap(i, j);

                if (curCycle.getCost() < bestCycle.getCost()) {
                    bestCycle.setPath(curCycle);
                    updated = true;
                }

                curCycle.swap(i, j);
            }
        }

        evals += evalsPerLoop;
    } while (updated);

    data.setPath(bestCycle);
    return evals;
}

void localSearch(Cycle &data, unsigned int seed) {
    std::mt19937 generator(seed);
    localSearch(data, generator);
}

void localSearch(Cycle &data, std::mt19937 &generator) {
    data.shufflePath(generator);
    localSearch(data);
}

} // namespace Algorithms
