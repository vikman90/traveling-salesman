/**
 * @file annealing.cpp
 * @brief Implementation of Simulated Annealing (SA) metaheuristic
 */

#include <cmath>
#include "../core/cycle.h"
#include "algorithms.h"

namespace {
constexpr double P = 0.249175;   ///< Initial temperature factor
constexpr int NCOOL = 20;        ///< Iterations per temperature stage
constexpr double TEMPEND = 0.1;  ///< Final minimum temperature

inline bool accept(double delta, double temperature, std::mt19937 &gen) {
    if (delta < 0.0) return true;
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(gen) < std::exp(-delta / temperature);
}
} // anonymous namespace

namespace Algorithms {

void simulatedAnnealing(Cycle &data, int count, unsigned int seed, NeighborGenerator neighbor) {
    std::mt19937 generator(seed);
    data.shufflePath(generator);
    simulatedAnnealing(data, count, generator, neighbor);
}

void simulatedAnnealing(Cycle &data, int count, std::mt19937 &generator, NeighborGenerator neighbor) {
    const int n = data.getSize();
    if (n < 3) return;

    const int nMax = count * n;
    Cycle &bestCycle = data;
    Cycle auxCycle = bestCycle;

    double temperature = P * bestCycle.getCost();
    const double beta = (temperature - TEMPEND) / (nMax * temperature * TEMPEND);

    for (int k = 0; k < nMax; k += NCOOL) {
        switch (neighbor) {
        case Swap: {
            for (int step = 0; step < NCOOL; ++step) {
                int i = random(generator, n);
                int j;
                do {
                    j = random(generator, n);
                } while (i == j);

                auxCycle.swap(i, j);

                if (accept(auxCycle.getCost() - bestCycle.getCost(), temperature, generator)) {
                    bestCycle.setPath(auxCycle);
                } else {
                    auxCycle.swap(i, j);
                }
            }
            break;
        }

        case Invert: {
            for (int step = 0; step < NCOOL; ++step) {
                int countSub = random(generator, 5) + 2;
                int begin = random(generator, n - countSub);

                auxCycle.invertSubpath(begin, countSub);

                if (accept(auxCycle.getCost() - bestCycle.getCost(), temperature, generator)) {
                    bestCycle.setPath(auxCycle);
                } else {
                    auxCycle.invertSubpath(begin, countSub);
                }
            }
            break;
        }
        }

        temperature /= (1.0 + beta * temperature);
    }
}

} // namespace Algorithms
