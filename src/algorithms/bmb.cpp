/**
 * @file bmb.cpp
 * @brief Implementation of Basic Multiboot (BMB) search
 */

#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

void basicMultibootSearch(Cycle &data, int count, unsigned int seed) {
    Cycle bestCycle = data;
    Cycle curCycle = data;
    std::mt19937 generator(seed);

    bestCycle.clearPath();

    for (int k = 0; k < count; ++k) {
        curCycle.shufflePath(generator);
        localSearch(curCycle); // Fixed: previously incorrectly operated on data

        if (curCycle.getCost() < bestCycle.getCost()) {
            bestCycle.setPath(curCycle);
        }
    }

    data.setPath(bestCycle);
}

} // namespace Algorithms
