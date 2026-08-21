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
    Cycle greedyCycle = data;
    Cycle curCycle = data;
    Cycle bestCycle = data;

    bestCycle.clearPath();
    greedy(greedyCycle);

    for (int k = 0; k < count; ++k) {
        curCycle.setPath(greedyCycle);
        curCycle.shuffleSubpath(s, generator);
        localSearch(curCycle);

        if (curCycle.getCost() < bestCycle.getCost()) {
            bestCycle.setPath(curCycle);
        }
    }

    data.setPath(bestCycle);
}

} // namespace Algorithms
