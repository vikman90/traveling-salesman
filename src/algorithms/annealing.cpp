/**
 * @file annealing.cpp
 * @brief Implementation of Simulated Annealing (SA) metaheuristic
 * @author Vikman Fernandez-Castro
 * @date Created: December 3, 2012
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#include <cmath>
#include "../core/cycle.h"
#include "algorithms.h"

namespace {
constexpr double P = 0.249175;     ///< Initial temperature factor (mu / -ln(phi) with mu=0.3, phi=0.3)
constexpr int NCOOL = 20;          ///< Iterations per temperature stage (epoch length L)
constexpr double TEMPEND = 0.001;  ///< Final minimum temperature (T_f = 10^-3)

inline bool accept(double delta, double temperature, std::mt19937 &gen) {
    if (delta <= 0.0) return true;
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
    Cycle curCycle = data;
    Cycle bestCycle = data;
    Cycle auxCycle = curCycle;

    double temperature = P * curCycle.getCost();
    const double beta = (temperature - TEMPEND) / (nMax * temperature * TEMPEND);

    for (int k = 0; k < nMax; k += NCOOL) {
        switch (neighbor) {
        case Swap: {
            for (int step = 0; step < NCOOL; ++step) {
                int i = random(generator, n);
                int j = random(generator, n);
                while (i == j) {
                    j = random(generator, n);
                }

                auxCycle.swap(i, j);
                double delta = auxCycle.getCost() - curCycle.getCost();

                if (accept(delta, temperature, generator)) {
                    curCycle.setPath(auxCycle);
                    if (curCycle.getCost() < bestCycle.getCost()) {
                        bestCycle.setPath(curCycle);
                    }
                } else {
                    auxCycle.swap(i, j); // Revert move
                }
            }
            break;
        }

        case Invert: {
            for (int step = 0; step < NCOOL; ++step) {
                int i = random(generator, n);
                int j = random(generator, n);
                while (i == j) {
                    j = random(generator, n);
                }
                if (i > j) std::swap(i, j);
                int countSub = j - i + 1;

                auxCycle.invertSubpath(i, countSub);
                double delta = auxCycle.getCost() - curCycle.getCost();

                if (accept(delta, temperature, generator)) {
                    curCycle.setPath(auxCycle);
                    if (curCycle.getCost() < bestCycle.getCost()) {
                        bestCycle.setPath(curCycle);
                    }
                } else {
                    auxCycle.invertSubpath(i, countSub); // Revert move
                }
            }
            break;
        }
        }

        temperature /= (1.0 + beta * temperature);
    }

    data.setPath(bestCycle);
}

} // namespace Algorithms
