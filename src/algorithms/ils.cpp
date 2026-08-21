/**
 * @file ils.cpp
 * @brief Implementation of Iterated Local Search (ILS)
 * @author Vikman Fernandez-Castro
 * @date Created: January 8, 2013
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#include <algorithm>
#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

void iteratedLocalSearch(Cycle &data, int count, unsigned int seed) {
    const int s = std::max(2, data.getSize() / 4);
    Cycle bestCycle = data;
    Cycle curCycle = data;
    std::mt19937 generator(seed);

    // Initial random descent
    localSearch(bestCycle, generator);

    for (int k = 1; k < count; ++k) {
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
