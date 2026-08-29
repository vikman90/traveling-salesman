/**
 * @file tabusearch.cpp
 * @brief Implementation of Tabu Search (BT) with Short-Term Memory and Diversification Restarts
 * @author Vikman Fernandez-Castro
 * @date Created: August 2026
 */

#include <algorithm>
#include <deque>
#include <limits>
#include <random>
#include <vector>

#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

namespace {

struct TabuEntry {
    int elem1;
    int elem2;
    int pos1;
    int pos2;
};

// Check whether placing elem1 at newPos1 OR elem2 at newPos2 violates any active tabu restriction
bool isMoveTabu(const std::deque<TabuEntry> &tabuList, int elem1, int elem2, int newPos1, int newPos2) {
    for (const auto &entry : tabuList) {
        if ((elem1 == entry.elem1 && newPos1 == entry.pos1) ||
            (elem1 == entry.elem2 && newPos1 == entry.pos2) ||
            (elem2 == entry.elem1 && newPos2 == entry.pos1) ||
            (elem2 == entry.elem2 && newPos2 == entry.pos2)) {
            return true;
        }
    }
    return false;
}

} // anonymous namespace

void tabuSearch(Cycle &data, int count, unsigned int seed) {
    std::mt19937 generator(seed);
    tabuSearch(data, count, generator);
}

void tabuSearch(Cycle &data, int count, std::mt19937 &generator) {
    const int n = data.getSize();
    if (n < 3) return;

    const int totalIters = count * n;
    const int numReinits = 4;
    const int reinitInterval = std::max(1, totalIters / (numReinits + 1)); // 50n / 5 = 10n

    int tabuTenure = std::max(1, n / 2);
    std::deque<TabuEntry> tabuList;

    // Long-term memory: frequency of city u assigned to position l in accepted solutions
    std::vector<std::vector<int>> freq(n, std::vector<int>(n, 0));

    Cycle curCycle = data;
    curCycle.shufflePath(generator);
    Cycle bestCycle = curCycle;

    // Record initial configuration in frequency table
    for (int l = 0; l < n; ++l) {
        freq[l][curCycle.edgeAt(l)]++;
    }

    std::uniform_real_distribution<double> uniform01(0.0, 1.0);

    for (int iter = 1; iter <= totalIters; ++iter) {
        // Reinitialization check at every reinitInterval iterations (4 times in total)
        if (iter % reinitInterval == 0 && iter < totalIters) {
            double p = uniform01(generator);
            if (p < 0.25) {
                // Strategy 1 (25%): Random initial solution
                curCycle.shufflePath(generator);
            } else if (p < 0.50) {
                // Strategy 2 (25%): Best solution obtained so far
                curCycle.setPath(bestCycle);
            } else {
                // Strategy 3 (50%): Long-term memory greedy diversification
                std::vector<bool> availL(n, true);
                std::vector<bool> availU(n, true);

                for (int step = 0; step < n; ++step) {
                    int minFreq = std::numeric_limits<int>::max();
                    int bestL = -1;
                    int bestU = -1;

                    for (int l = 0; l < n; ++l) {
                        if (!availL[l]) continue;
                        for (int u = 0; u < n; ++u) {
                            if (!availU[u]) continue;
                            if (freq[l][u] < minFreq) {
                                minFreq = freq[l][u];
                                bestL = l;
                                bestU = u;
                            }
                        }
                    }

                    if (bestL != -1 && bestU != -1) {
                        curCycle.edgeAt(bestL) = bestU;
                        availL[bestL] = false;
                        availU[bestU] = false;
                    }
                }
                curCycle.updateCost();
            }

            if (curCycle.getCost() < bestCycle.getCost()) {
                bestCycle.setPath(curCycle);
            }

            // Tabu list tenure modification: ±50% with uniform probability
            if (uniform01(generator) < 0.5) {
                tabuTenure = std::max(1, static_cast<int>(tabuTenure * 1.5));
            } else {
                tabuTenure = std::max(1, static_cast<int>(tabuTenure * 0.5));
            }

            // Trim tabu list to new tenure
            while (static_cast<int>(tabuList.size()) > tabuTenure) {
                tabuList.pop_front();
            }
        }

        // Neighborhood exploration: evaluate 40 candidate neighbors
        const int maxPossiblePairs = (n * (n - 1)) / 2;
        const int numCandidates = std::min(40, maxPossiblePairs);

        int bestP1 = -1;
        int bestP2 = -1;
        float bestNeighborCost = std::numeric_limits<float>::max();

        int fallbackP1 = -1;
        int fallbackP2 = -1;
        float fallbackCost = std::numeric_limits<float>::max();

        for (int c = 0; c < numCandidates; ++c) {
            int p1 = random(generator, n);
            int p2 = random(generator, n);
            while (p1 == p2) {
                p2 = random(generator, n);
            }
            if (p1 > p2) std::swap(p1, p2);

            int elem1 = curCycle.edgeAt(p1);
            int elem2 = curCycle.edgeAt(p2);

            curCycle.swap(p1, p2);
            float candidateCost = curCycle.getCost();
            curCycle.swap(p1, p2); // Revert swap

            // Track overall best candidate as fallback if all are tabu
            if (candidateCost < fallbackCost) {
                fallbackCost = candidateCost;
                fallbackP1 = p1;
                fallbackP2 = p2;
            }

            // Aspiration criterion: lower cost than the overall best solution found so far
            bool aspiration = (candidateCost < bestCycle.getCost());
            bool tabu = false;

            if (!aspiration) {
                // In candidate move, elem1 moves to p2, elem2 moves to p1
                tabu = isMoveTabu(tabuList, elem1, elem2, p2, p1);
            }

            if (!tabu && candidateCost < bestNeighborCost) {
                bestNeighborCost = candidateCost;
                bestP1 = p1;
                bestP2 = p2;
            }
        }

        // If all candidate neighbors were tabu and none met aspiration, accept best fallback
        if (bestP1 == -1) {
            bestP1 = fallbackP1;
            bestP2 = fallbackP2;
        }

        if (bestP1 != -1 && bestP2 != -1) {
            int elem1 = curCycle.edgeAt(bestP1);
            int elem2 = curCycle.edgeAt(bestP2);

            // Execute move
            curCycle.swap(bestP1, bestP2);

            // Update best global solution
            if (curCycle.getCost() < bestCycle.getCost()) {
                bestCycle.setPath(curCycle);
            }

            // Record move in tabu list: elem1 is now at bestP2, elem2 is at bestP1
            tabuList.push_back({elem1, elem2, bestP2, bestP1});
            while (static_cast<int>(tabuList.size()) > tabuTenure) {
                tabuList.pop_front();
            }

            // Update frequency matrix for long-term memory
            for (int l = 0; l < n; ++l) {
                freq[l][curCycle.edgeAt(l)]++;
            }
        }
    }

    data.setPath(bestCycle);
}

} // namespace Algorithms
