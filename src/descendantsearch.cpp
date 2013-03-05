// 7 de Noviembre de 2012

#include <climits>
#include <cfloat>
#include "cycle.h"
#include "algorithms.h"

namespace Algorithms
{
void descendantSearch(Cycle &data, int count, unsigned int seed)
{
    const int N1 = data.getSize() * (data.getSize() - 1) / 2;
    const int N2 = 24 * (data.getSize() - 3);
    const int N3 = 360 * (data.getSize() - 5);
    const int NMAX = count * data.getSize();
    int nTotal = 1;
    int k = 1;
    std::mt19937 generator(seed);
    Cycle &curCycle = data;     // Solución actual (a optimizar)
    Cycle auxCycle = data;      // Solución vecina auxiliar
    Cycle bestNeighbor = data;  // Mejor solución vecina

    // Generar solución inicial

    curCycle.shufflePath(generator);

    do {
        bestNeighbor.setPath(curCycle);
        auxCycle.setPath(curCycle);

        switch (k) {
        case 1: // k = 1 -> Intercambio
            nTotal += N1;

            for (int i = 1; i < data.getSize(); i++) {
                for (int j = 0; j < i; j++) {
                    auxCycle.swap(i, j);

                    if (bestNeighbor.getCost() > auxCycle.getCost())
                        bestNeighbor.setPath(auxCycle);

                    auxCycle.swap(i, j);
                }
            }

            break;

        case 2: // k = 2 -> Sublista aleatoria de tamaño 4
            nTotal += N2;

            for (int i = 0; i < N2; i++) {
                auxCycle.shuffleSubpath(4, generator);

                if (bestNeighbor.getCost() > auxCycle.getCost())
                    bestNeighbor.setPath(auxCycle);

                auxCycle.setPath(curCycle);
            }

            break;

        case 3: // k = 3 -> Sublista aleatoria de tamaño 6
            nTotal += N3;

            for (int i = 0; i < N3; i++) {
                auxCycle.shuffleSubpath(6, generator);

                if (bestNeighbor.getCost() > auxCycle.getCost())
                    bestNeighbor.setPath(auxCycle);

                auxCycle.setPath(curCycle);
            }
        }

        if (curCycle.getCost() > bestNeighbor.getCost()) {
            curCycle.setPath(bestNeighbor);
            k = 1;
        } else
            k++;

    } while (k <= 3 && nTotal <= NMAX);
}
}
