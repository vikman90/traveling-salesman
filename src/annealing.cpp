// December 3, 2012

#include "algorithms.h"
#include <cmath>
#include "cycle.h"

#define P 0.249175  ///< Multiple of the initial temperature
#define NCOOL 20    ///< Number of iterations at each temperature state
#define TEMPEND 0.1 ///< Final temperature

/// Solution acceptance condition
inline static bool accept(double delta, double temperature, std::mt19937 &gen)
{
    if (delta < 0.0)
        return true;
    else
        return (double)gen() / gen.max() < exp(-delta / temperature);
}

namespace Algorithms
{
void simulatedAnnealing(Cycle &data, int count, unsigned int seed, NeighborGenerator neighbor)
{
    std::mt19937 generator(seed);

    // Generate initial solution
    data.shufflePath(generator);

    simulatedAnnealing(data, count, generator, neighbor);
}

void simulatedAnnealing(Cycle &data, int count, std::mt19937 &generator, NeighborGenerator neighbor)
{
    const int nMax = count * data.getSize();
    double temperature;
    Cycle &bestCycle = data;    // Cycle to optimize
    Cycle auxCycle = bestCycle; // Candidate cycle

    // Set temperature

    temperature = P * bestCycle.getCost();
    const double beta = (temperature - TEMPEND) / (nMax * temperature * TEMPEND);

    for (int k = 0; k < nMax; k += NCOOL) {
        switch (neighbor) {
        case Swap:
            int i, j;

            for (int n = 0; n < NCOOL; n++) {

                // Random in [0, size[
                i = random(generator, auxCycle.getSize());

                do {
                    j = random(generator, auxCycle.getSize());
                } while (i == j);

                auxCycle.swap(i, j);

                if (accept(auxCycle.getCost() - bestCycle.getCost(), temperature, generator)) {
                    bestCycle.setPath(auxCycle);
                } else {
                    auxCycle.swap(i, j);
                }
            }

            break;

        case Invert:
            int begin, count;

            for (int n = 0; n < NCOOL; n++) {

                // Random in [2, 6]
                count = random(generator, 5) + 2;

                begin = random(generator, auxCycle.getSize() - count);
                auxCycle.invertSubpath(begin, count);

                if (accept(auxCycle.getCost() - bestCycle.getCost(), temperature, generator)) {
                    bestCycle.setPath(auxCycle);
                } else {
                    auxCycle.invertSubpath(begin, count);
                }
            }
        }

        temperature /= (1.0 + beta * temperature);
    }
}
}
