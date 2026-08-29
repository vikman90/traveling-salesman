/**
 * @file greedybl.cpp
 * @brief Implementation of Greedy + Local Search and Extended Greedy Search
 * @author Vikman Fernandez-Castro
 * @date Created: January 7, 2013
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#include <algorithm>
#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

void greedy_bl(Cycle &data) {
    greedy(data);
    localSearch(data);
}

void greedy_blext(Cycle &data, int count, unsigned int seed) {
    const int s = std::max(2, data.getSize() / 4);
    std::mt19937 generator(seed);
    Cycle bestCycle = data;
    Cycle curCycle = data;

    // 1. Initial deterministic greedy tour + first Local Search (1st BL)
    greedy(bestCycle);
    localSearch(bestCycle);

    // 2. Perform (count - 1) iterative perturbation + local search cycles (default count=5 -> 4 cycles)
    const int numCycles = std::max(0, count - 1);
    for (int k = 0; k < numCycles; ++k) {
        curCycle.setPath(bestCycle);
        curCycle.shuffleSubpath(s, generator);
        localSearch(curCycle);

        if (curCycle.getCost() < bestCycle.getCost()) {
            bestCycle.setPath(curCycle);
        }
    }

    data.setPath(bestCycle);
}

} // namespace Algorithms
