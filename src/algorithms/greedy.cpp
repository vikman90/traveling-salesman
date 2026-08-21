/**
 * @file greedy.cpp
 * @brief Implementation of Greedy Nearest Neighbor Heuristic
 * @author Vikman Fernandez-Castro
 * @date Created: October 23, 2012
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#include <cfloat>
#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

void greedy(Cycle &data) {
    const int n = data.getSize();
    if (n < 2) return;

    Cycle curCycle = data;
    Cycle bestCycle = data;
    bestCycle.clearPath();

    for (int init = 0; init < n; ++init) {
        curCycle.sortPath();

        if (init != 0) {
            curCycle.swap(0, init);
        }

        for (int i = 1; i < n - 1; ++i) {
            int last = curCycle.edgeAt(i - 1);
            float bestDist = FLT_MAX;
            int nearestEdge = i;

            for (int j = i; j < n; ++j) {
                float d = data.distance(last, curCycle.edgeAt(j));
                if (d < bestDist) {
                    bestDist = d;
                    nearestEdge = j;
                }
            }

            curCycle.swap(i, nearestEdge);
        }

        if (curCycle.getCost() < bestCycle.getCost()) {
            bestCycle.setPath(curCycle);
        }
    }

    data.setPath(bestCycle);
}

} // namespace Algorithms
