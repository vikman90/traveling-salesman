/**
 * @file grasp.cpp
 * @brief Implementation of Greedy Randomized Adaptive Search Procedure (GRASP)
 * @author Vikman Fernandez-Castro
 * @date Created: January 7, 2013
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#include <algorithm>
#include <vector>
#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

namespace {

constexpr int NMUT = 4; ///< Number of mutation cycles in GRASP+ (1 initial BL + 4 mutation BLs = 5 BLs/start)

void greedyProb(Cycle &data, std::mt19937 &generator) {
    const int n = data.getSize();
    if (n < 3) return;

    const int lsize = std::max(2, n / 10);

    data.sortPath();
    // Choose initial city at random
    int startIdx = random(generator, n);
    data.swap(0, startIdx);

    for (int i = 1; i < n; ++i) {
        int lastNode = data.edgeAt(i - 1);
        int rem = n - i;
        int currentL = std::min(rem, lsize);

        // Select the currentL closest unvisited cities among ALL remaining unvisited nodes
        std::vector<int> &edges = const_cast<std::vector<int>&>(data.getEdges());
        std::partial_sort(edges.begin() + i, edges.begin() + i + currentL, edges.end(),
            [&](int a, int b) {
                return data.distance(lastNode, a) < data.distance(lastNode, b);
            });

        // Pick uniformly at random from the RCL
        int choiceOffset = random(generator, currentL);
        if (choiceOffset != 0) {
            data.swap(i, i + choiceOffset);
        }
    }

    data.updateCost();
}

} // anonymous namespace

void grasp(Cycle &data, int count, unsigned int seed) {
    Cycle bestCycle = data;
    Cycle curCycle = data;
    std::mt19937 generator(seed);

    bestCycle.clearPath();

    for (int k = 0; k < count; ++k) {
        greedyProb(curCycle, generator);
        localSearch(curCycle);

        if (curCycle.getCost() < bestCycle.getCost()) {
            bestCycle.setPath(curCycle);
        }
    }

    data.setPath(bestCycle);
}

void graspExt(Cycle &data, int count, unsigned int seed) {
    const int s = std::max(2, data.getSize() / 4);
    Cycle bestCycle = data;
    Cycle curCycle = data;
    std::mt19937 generator(seed);

    bestCycle.clearPath();

    for (int k = 0; k < count; ++k) {
        greedyProb(curCycle, generator);
        localSearch(curCycle);

        for (int m = 0; m < NMUT; ++m) {
            curCycle.shuffleSubpath(s, generator);
            localSearch(curCycle);

            if (curCycle.getCost() < bestCycle.getCost()) {
                bestCycle.setPath(curCycle);
            }
        }
    }

    data.setPath(bestCycle);
}

} // namespace Algorithms
