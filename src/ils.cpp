// 8 de Enero de 2013

#include "cycle.h"
#include "algorithms.h"

namespace Algorithms
{

void iteratedLocalSearch(Cycle &data, int count, unsigned int seed)
{
    const int s = data.getSize() / 4;
    Cycle &bestCycle = data;
    Cycle curCycle = data;
    std::mt19937 generator(seed);

    // Esta sobrecarga inicializa aleatoriamente
    localSearch(bestCycle, generator);

    for (int k = 1; k < count; k++) {
        curCycle.setPath(bestCycle);
        curCycle.shuffleSubpath(s, generator);
        localSearch(curCycle);

        if (curCycle.getCost() < bestCycle.getCost())
            bestCycle.setPath(curCycle);
    }
}

}
