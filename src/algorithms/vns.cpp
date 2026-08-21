/**
 * @file vns.cpp
 * @brief Implementation of Variable Neighborhood Search (VNS)
 */

#include <algorithm>
#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

namespace {
constexpr int KMAX = 5;
}

void variableSearch(Cycle &data, int count, unsigned int seed) {
    const int n = data.getSize();
    if (n < 4) return;

    Cycle bestCycle = data;
    Cycle curCycle = data;
    std::mt19937 generator(seed);
    int k = 1;
    int bl = 0;

    bestCycle.shufflePath(generator);

    do {
        if (k > KMAX) {
            k = 1;
        }

        curCycle.setPath(bestCycle);
        int subLen = std::max(2, n / std::max(1, (9 - k)));
        curCycle.shuffleSubpath(subLen, generator);
        localSearch(curCycle);

        if (curCycle.getCost() < bestCycle.getCost()) {
            bestCycle.setPath(curCycle);
            k = 1;
        } else {
            k++;
        }
    } while (++bl < count);

    data.setPath(bestCycle);
}

} // namespace Algorithms
