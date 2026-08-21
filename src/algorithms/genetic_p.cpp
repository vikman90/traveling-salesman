/**
 * @file genetic_p.cpp
 * @brief Implementation of Parallel Genetic Algorithm with Island Migration Models
 */

#include <cfloat>
#include <vector>
#include "../core/cycle.h"
#include "../core/population.h"
#include "algorithms.h"

namespace Algorithms {

void parallelGenetic(Cycle &data, int processes, int size, int count, int migrLatency, Topology topology, unsigned int seed) {
    if (processes < 1) processes = 1;
    if (size < 3 || data.getSize() < 3) return;
    if (migrLatency < 1) migrLatency = 1;

    const int nMax = count * data.getSize();
    std::mt19937 generator(seed);
    Cycle &bestCycle = data;

    std::vector<Population> world;
    world.reserve(processes);
    for (int i = 0; i < processes; ++i) {
        world.emplace_back(size, data, generator);
    }

    const int migrationInterval = migrLatency * data.getSize();

    for (int n = 0; n < nMax; ++n) {
        for (int i = 0; i < processes; ++i) {
            world[i].evolve(Generational);
        }

        // Migration step
        if ((n + 1) % migrationInterval == 0 && processes > 1) {
            switch (topology) {
            case Star: {
                float bestMean = FLT_MAX;
                int iBest = 0;

                for (int i = 0; i < processes; ++i) {
                    float mean = world[i].meanCost();
                    if (mean < bestMean) {
                        iBest = i;
                        bestMean = mean;
                    }
                }

                for (int i = 0; i < processes; ++i) {
                    if (i != iBest) {
                        world[i][world[i].worstCycle()].setPath(world[iBest][world[iBest].bestCycle()]);
                        world[iBest][world[iBest].worstCycle()].setPath(world[i][world[i].bestCycle()]);
                    }
                }
                break;
            }

            case Ring: {
                for (int i = 0; i < processes; ++i) {
                    int nextIsland = (i + 1) % processes;
                    world[i][world[i].worstCycle()].setPath(world[nextIsland][world[nextIsland].bestCycle()]);
                }
                break;
            }
            }
        }

        // Global best tracking
        for (int i = 0; i < processes; ++i) {
            int iBest = world[i].bestCycle();
            if (world[i][iBest].getCost() < bestCycle.getCost()) {
                bestCycle.setPath(world[i][iBest]);
            }
        }
    }
}

} // namespace Algorithms
