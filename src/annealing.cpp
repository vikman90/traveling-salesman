// 3 de Diciembre de 2012

#include "algorithms.h"
#include <cmath>
#include "cycle.h"

#define P 0.249175  ///< Múltiplo de la temperatura inicial
#define NCOOL 20    ///< Número de iteraciones en cada estado de temperatura
#define TEMPEND 0.1 ///< Temperatura final

/// Condición de aceptación de solución
inline static bool accept(double delta, double temperature, std::mt19937 &gen)
{
    if (delta < 0.0)
        return true;
    else
        return (double)gen() / gen.max() < exp(-delta / temperature);
}

namespace Algorithms {
void simulatedAnnealing(Cycle &data, int count, unsigned int seed, NeighborGenerator neighbor)
{
	std::mt19937 generator(seed);

	// Generar solucion inicial
	data.shufflePath(generator);

	simulatedAnnealing(data, count, generator, neighbor);
}

void simulatedAnnealing(Cycle &data, int count, std::mt19937 &generator, NeighborGenerator neighbor)
{
    const int nMax = count * data.getSize();
    double temperature;
    Cycle &bestCycle = data;    // Ciclo a optimizar
    Cycle auxCycle = bestCycle; // Ciclo candidato

    // Establecer temperatura

    temperature = P * bestCycle.getCost();
    const double beta = (temperature - TEMPEND) / (nMax * temperature * TEMPEND);

    for (int k = 0; k < nMax; k += NCOOL) {
        switch (neighbor) {
        case Swap:
            int i, j;

            for (int n = 0; n < NCOOL; n++) {

                // Aleatorio en [0, size[
                i = random(generator, auxCycle.getSize());

                do {
                    j = random(generator, auxCycle.getSize());
                } while (i == j);

                auxCycle.swap(i, j);

                if (accept(auxCycle.getCost() - bestCycle.getCost(), temperature, generator)) {
                    bestCycle.setPath(auxCycle);
                }
                else {
                    auxCycle.swap(i, j);
                }
            }

            break;

        case Invert:
            int begin, count;

            for (int n = 0; n < NCOOL; n++) {

                // Aleatorio en [2, 6]
                count = random(generator, 5) + 2;

                begin = random(generator, auxCycle.getSize() - count);
                auxCycle.invertSubpath(begin, count);

                if (accept(auxCycle.getCost() - bestCycle.getCost(), temperature, generator)) {
                    bestCycle.setPath(auxCycle);
                }
                else {
                    auxCycle.invertSubpath(begin, count);
                }
            }
        }

        temperature /= (1.0 + beta * temperature);
    }
}
}
