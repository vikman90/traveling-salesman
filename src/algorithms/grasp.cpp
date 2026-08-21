/**
 * @file grasp.cpp
 * @brief Implementation of Greedy Randomized Adaptive Search Procedure (GRASP)
 */

#include <algorithm>
#include <vector>
#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

namespace {

constexpr int NMUT = 5; ///< Number of mutations in GRASP+

void sortCandidates(Cycle &data, int ilast, int lsize) {
    const int begin = ilast + 1;
    const int end = begin + lsize;
    const int lastNode = data.edgeAt(ilast);

    std::vector<int> candidates(end - begin);
    for (int i = 0; i < end - begin; ++i) {
        candidates[i] = data.edgeAt(begin + i);
    }

    std::sort(candidates.begin(), candidates.end(), [&](int a, int b) {
        return data.distance(lastNode, a) < data.distance(lastNode, b);
    });

    for (int i = 0; i < end - begin; ++i) {
        data.edgeAt(begin + i) = candidates[i];
    }
    data.updateCost();
}

void greedyProb(Cycle &data, std::mt19937 &generator) {
    const int n = data.getSize();
    if (n < 3) return;

    const int lsize = std::max(2, n / 10);
    int ilast = 0;

    data.sortPath();
    data.swap(0, random(generator, n));

    while (n - ilast - 1 > lsize) {
        sortCandidates(data, ilast, lsize);
        data.swap(ilast, random(generator, lsize) + ilast + 1);
        ilast++;
    }

    while (n - ilast > 2) {
        data.swap(ilast, random(generator, n - ilast - 1) + ilast + 1);
        ilast++;
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
