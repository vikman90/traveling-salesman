/**
 * @file randomsearch.cpp
 * @brief Implementation of Pure Random Search heuristic
 */

#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

void randomSearch(Cycle &data, int count, unsigned int seed) {
    const int maxIters = count * data.getSize();
    Cycle curCycle = data;
    Cycle bestCycle = data;
    std::mt19937 generator(seed);

    bestCycle.shufflePath(generator);

    for (int i = 1; i < maxIters; ++i) {
        curCycle.shufflePath(generator);

        if (curCycle.getCost() < bestCycle.getCost()) {
            bestCycle.setPath(curCycle);
        }
    }

    data.setPath(bestCycle);
}

} // namespace Algorithms
