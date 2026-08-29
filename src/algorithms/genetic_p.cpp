/**
 * @file genetic_p.cpp
 * @brief Implementation of Parallel Genetic Algorithm (PGA) Island Model
 * @author Vikman Fernandez-Castro
 * @date Created: March 3, 2013
 * @date Modified: August 2026 (C++17 STL Modernization)
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

    const int totalGenerations = count * data.getSize();
    const int migrationInterval = std::max(1, migrLatency * data.getSize());

    std::mt19937 generator(seed);
    Cycle bestCycle = data;
    bestCycle.clearPath();

    std::vector<Population> world;
    world.reserve(processes);
    for (int i = 0; i < processes; ++i) {
        world.emplace_back(size, data, generator);
        int iBest = world[i].bestCycle();
        if (world[i][iBest].getCost() < bestCycle.getCost()) {
            bestCycle.setPath(world[i][iBest]);
        }
    }

    for (int gen = 1; gen <= totalGenerations; ++gen) {
        for (int i = 0; i < processes; ++i) {
            world[i].evolve(Generational);
        }

        // Migration step
        if (gen % migrationInterval == 0 && processes > 1) {
            switch (topology) {
            case Star: {
                float bestMean = FLT_MAX;
                int masterIdx = 0;

                for (int i = 0; i < processes; ++i) {
                    float mean = world[i].meanCost();
                    if (mean < bestMean) {
                        masterIdx = i;
                        bestMean = mean;
                    }
                }

                Cycle masterBest = world[masterIdx][world[masterIdx].bestCycle()];
                for (int i = 0; i < processes; ++i) {
                    if (i != masterIdx) {
                        Cycle slaveBest = world[i][world[i].bestCycle()];
                        world[i][world[i].worstCycle()].setPath(masterBest);
                        world[masterIdx][world[masterIdx].worstCycle()].setPath(slaveBest);
                    }
                }
                break;
            }

            case Ring: {
                std::vector<Cycle> champions(processes);
                for (int i = 0; i < processes; ++i) {
                    champions[i] = world[i][world[i].bestCycle()];
                }
                for (int i = 0; i < processes; ++i) {
                    int targetIsland = (i + 1) % processes;
                    world[targetIsland][world[targetIsland].worstCycle()].setPath(champions[i]);
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

    data.setPath(bestCycle);
}

} // namespace Algorithms
