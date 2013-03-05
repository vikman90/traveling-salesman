// 7 de Enero de 2013

#include <cfloat>
#include "cycle.h"
#include "algorithms.h"

#define NMUT 5  ///< Numero de mutaciones por ciclo en GRASP+

namespace Algorithms
{

/**
 * @brief Ordenacion por burbuja parcial
 * @param data Ciclo que manipulamos
 * @param ilast Indice del ultimo nodo añadido (a comparar)
 * @param lsize Longitud de la lista parcial (ordenar @p lsize elementos)
 */
void bubblesort(Cycle &data, int ilast, int lsize)
{
    const int begin = ilast + 1;
    const int end = begin + lsize;
    const int lastNode = data.edgeAt(ilast);
    bool modificado = true;

    for (int i = begin; i < end && modificado; i++) {
        modificado = false;

        for (int j = end - 1; j > i; j--) {
            if (data.distance(lastNode, data.edgeAt(j - 1)) > data.distance(lastNode, data.edgeAt(j))) {
                data.swap(j, j - 1);
                modificado = true;
            }
        }
    }
}

void greedyProb(Cycle &data, std::mt19937 &generator)
{
    const int lsize = data.getSize() / 10;
    int ilast = 0;

    data.sortPath();

    // Ciudad inicial

    data.swap(0, random(generator, data.getSize()));

    // Mientras haya mas ciudades que 'lsize', habra que reordenar

    while (data.getSize() - ilast - 1 > lsize) {
        bubblesort(data, ilast, lsize);
        data.swap(ilast, random(generator, lsize) + ilast + 1);
        ilast++;
    }

    // Seguimos sin reordenar

    while (data.getSize() - ilast > 2) {
        data.swap(ilast, random(generator, data.getSize() - ilast - 1) + ilast + 1);
        ilast++;
    }
}

void grasp(Cycle &data, int count, unsigned int seed)
{
    Cycle &bestCycle = data;
    Cycle curCycle = data;
    std::mt19937 generator(seed);

    bestCycle.clearPath();

    for (int k = 0; k < count; k++) {
        greedyProb(curCycle, generator);
        localSearch(curCycle);

        if (curCycle.getCost() < bestCycle.getCost())
            bestCycle.setPath(curCycle);
    }
}

void graspExt(Cycle &data, int count, unsigned int seed)
{
    const int s = data.getSize() / 4;
    Cycle &bestCycle = data;
    Cycle curCycle = data;
    std::mt19937 generator(seed);

    bestCycle.clearPath();

    for (int k = 0; k < count; k++) {
        greedyProb(curCycle, generator);
        localSearch(curCycle);

        for (int m = 0; m < NMUT; m++) {
            curCycle.shuffleSubpath(s, generator);
            localSearch(curCycle);

            if (bestCycle.getCost() > curCycle.getCost())
                bestCycle.setPath(curCycle);
        }
    }
}

}
