/**
 * @file algorithms.h
 * @brief Declarations and definitions of TSP heuristic and metaheuristic algorithms
 * @author Vikman Fernandez-Castro
 * @date Created: October 23, 2012
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#ifndef TSP_ALGORITHMS_H
#define TSP_ALGORITHMS_H

#include <climits>
#include <random>
#include "../core/cycle.h"

namespace Algorithms {

enum NeighborGenerator { Swap, Invert };
enum Scheme { Generational, Stationary };
enum Hybridization {
    Everygen_Twochrom,
    Everygen_Everychrom,
    Tengen_Twochrom,
    Tengen_Everychrom
};
enum Topology { Star, Ring };

/**
 * @brief Generate a uniform pseudo-random integer in the range [0, max[
 * @param generator Mersenne Twister engine reference.
 * @param max Upper bound (exclusive).
 * @return Random integer in [0, max - 1].
 */
inline int random(std::mt19937 &generator, int max) {
    if (max <= 1) return 0;
    std::uniform_int_distribution<int> dist(0, max - 1);
    return dist(generator);
}

// 1. Constructive Heuristics
void greedy(Cycle &data);
void greedy_bl(Cycle &data);
void greedy_blext(Cycle &data, int count, unsigned int seed);
void grasp(Cycle &data, int count, unsigned int seed);
void graspExt(Cycle &data, int count, unsigned int seed);

// 2. Neighborhood & Local Search
int localSearch(Cycle &data);
void localSearch(Cycle &data, unsigned int seed);
void localSearch(Cycle &data, std::mt19937 &generator);
void randomSearch(Cycle &data, int count, unsigned int seed);
void descendantSearch(Cycle &data, int count, unsigned int seed);
void basicMultibootSearch(Cycle &data, int count, unsigned int seed);
void iteratedLocalSearch(Cycle &data, int count, unsigned int seed);
void variableSearch(Cycle &data, int count, unsigned int seed);

// 3. Metaheuristics & Evolutionary
void simulatedAnnealing(Cycle &data, int count, unsigned int seed, NeighborGenerator neighbor);
void simulatedAnnealing(Cycle &data, int count, std::mt19937 &generator, NeighborGenerator neighbor);
void genetic(Cycle &data, int size, int count, Scheme scheme, unsigned int seed);
void memetic(Cycle &data, int size, int count, Hybridization hybridization, unsigned int seed);

// 4. Parallel Metaheuristics
void parallelAnnealing(Cycle &data, int processes, int count, int migrLatency, unsigned int seed);
void parallelGenetic(Cycle &data, int processes, int size, int count, int migrLatency, Topology topology, unsigned int seed);

} // namespace Algorithms

#endif // TSP_ALGORITHMS_H
