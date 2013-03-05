// 23 de Octubre de 2012

#include <cstdlib>
#include <climits>
#include <cfloat>
#include "cycle.h"
#include "algorithms.h"

namespace Algorithms {

void randomSearch(Cycle &data, int count, unsigned int seed)
{
    const int MAX = count * data.getSize();
    Cycle curCycle = data;
    Cycle &bestCycle = data;
    std::mt19937 generator(seed);

    /*
     * Generamos una solucion aleatoria (curCycle).
     * Si mejora a la mejor candidata, la guardamos.
     */

    bestCycle.shufflePath(generator);

    for (int i = 1; i < MAX; i++) {
        curCycle.shufflePath(generator);

        if (bestCycle.getCost() > curCycle.getCost())
            bestCycle.setPath(curCycle);
    }
}
}
