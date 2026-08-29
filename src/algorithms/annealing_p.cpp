/**
 * @file annealing_p.cpp
 * @brief Implementation of Parallel Simulated Annealing (PSA)
 * @author Vikman Fernandez-Castro
 * @date Created: March 3, 2013
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#include <vector>
#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

void parallelAnnealing(Cycle &data, int processes, int count, int migrLatency, unsigned int seed) {
    if (processes < 1) processes = 1;
    if (migrLatency < 1) migrLatency = 1;

    const int n = data.getSize();
    if (n < 3) return;

    // Parameters as specified in P4 §3.1:
    // Total coolings per process: count * n (default count = 20 -> 20 * n)
    // Neighbors per cooling step: L = 20
    // Migration epoch interval: migrLatency * n (default migrLatency = 1 -> n)
    const int totalCoolings = count * n;
    const int epochCoolings = migrLatency * n;
    constexpr int L = 20;
    constexpr double P_FACTOR = 0.249175;
    constexpr double TEMPEND = 0.001;

    std::mt19937 generator(seed);
    Cycle bestGlobal = data;
    bestGlobal.clearPath();

    struct IslandState {
        Cycle curCycle;
        Cycle auxCycle;
        double temperature{0.0};
        double beta{0.0};
    };

    std::vector<IslandState> islands(processes);
    for (int p = 0; p < processes; ++p) {
        islands[p].curCycle = data;
        islands[p].curCycle.shufflePath(generator);
        islands[p].auxCycle = islands[p].curCycle;
        islands[p].temperature = P_FACTOR * islands[p].curCycle.getCost();
        islands[p].beta = (islands[p].temperature - TEMPEND) / (totalCoolings * L * islands[p].temperature * TEMPEND);

        if (islands[p].curCycle.getCost() < bestGlobal.getCost()) {
            bestGlobal.setPath(islands[p].curCycle);
        }
    }

    std::uniform_real_distribution<double> uniform01(0.0, 1.0);

    for (int step = 0; step < totalCoolings; step += epochCoolings) {
        int stepsThisEpoch = std::min(epochCoolings, totalCoolings - step);

        // Run each process for this epoch
        for (int p = 0; p < processes; ++p) {
            auto &isl = islands[p];
            for (int s = 0; s < stepsThisEpoch; ++s) {
                for (int l = 0; l < L; ++l) {
                    int i = random(generator, n);
                    int j = random(generator, n);
                    while (i == j) {
                        j = random(generator, n);
                    }

                    isl.auxCycle.swap(i, j);
                    double delta = isl.auxCycle.getCost() - isl.curCycle.getCost();

                    if (delta <= 0.0 || uniform01(generator) < std::exp(-delta / isl.temperature)) {
                        isl.curCycle.setPath(isl.auxCycle);
                        if (isl.curCycle.getCost() < bestGlobal.getCost()) {
                            bestGlobal.setPath(isl.curCycle);
                        }
                    } else {
                        isl.auxCycle.swap(i, j); // Revert
                    }
                }
                isl.temperature /= (1.0 + isl.beta * isl.temperature);
            }
        }

        // Migration step: All processes adopt the global best solution so far
        for (int p = 0; p < processes; ++p) {
            islands[p].curCycle.setPath(bestGlobal);
            islands[p].auxCycle.setPath(bestGlobal);
        }
    }

    data.setPath(bestGlobal);
}

} // namespace Algorithms
