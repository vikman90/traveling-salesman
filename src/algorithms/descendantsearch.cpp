/**
 * @file descendantsearch.cpp
 * @brief Implementation of Variable Neighborhood Descent (VND)
 * @author Vikman Fernandez-Castro
 * @date Created: November 7, 2012
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

void descendantSearch(Cycle &data, int count, unsigned int seed) {
    const int n = data.getSize();
    if (n < 6) return;

    const int n1 = (n * (n - 1)) / 2;
    const int n2 = 24 * (n - 3);
    const int n3 = 360 * (n - 5);
    const int nMax = count * n;

    int nTotal = 1;
    int k = 1;
    std::mt19937 generator(seed);

    Cycle curCycle = data;
    Cycle auxCycle = data;
    Cycle bestNeighbor = data;

    curCycle.shufflePath(generator);

    do {
        bestNeighbor.setPath(curCycle);
        auxCycle.setPath(curCycle);

        switch (k) {
        case 1: // k = 1 -> Swap neighborhood
            nTotal += n1;
            for (int i = 1; i < n; ++i) {
                for (int j = 0; j < i; ++j) {
                    auxCycle.swap(i, j);

                    if (auxCycle.getCost() < bestNeighbor.getCost()) {
                        bestNeighbor.setPath(auxCycle);
                    }

                    auxCycle.swap(i, j);
                }
            }
            break;

        case 2: // k = 2 -> Size 4 random subpath
            nTotal += n2;
            for (int i = 0; i < n2; ++i) {
                auxCycle.shuffleSubpath(4, generator);

                if (auxCycle.getCost() < bestNeighbor.getCost()) {
                    bestNeighbor.setPath(auxCycle);
                }

                auxCycle.setPath(curCycle);
            }
            break;

        case 3: // k = 3 -> Size 6 random subpath
            nTotal += n3;
            for (int i = 0; i < n3; ++i) {
                auxCycle.shuffleSubpath(6, generator);

                if (auxCycle.getCost() < bestNeighbor.getCost()) {
                    bestNeighbor.setPath(auxCycle);
                }

                auxCycle.setPath(curCycle);
            }
            break;
        }

        if (bestNeighbor.getCost() < curCycle.getCost()) {
            curCycle.setPath(bestNeighbor);
            k = 1;
        } else {
            k++;
        }

    } while (k <= 3 && nTotal <= nMax);

    data.setPath(curCycle);
}

} // namespace Algorithms
