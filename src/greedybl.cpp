// 7 de Enero de 2013

#include "algorithms.h"
#include "cycle.h"

namespace Algorithms
{

void greedy_bl(Cycle &data)
{
    greedy(data);
    localSearch(data);
}

void greedy_blext(Cycle &data, int count, unsigned int seed)
{
    const int s = data.getSize() / 4;
    std::mt19937 generator(seed);
    Cycle greedyCycle = data;
    Cycle curCycle = data;
    Cycle &bestCycle = data;

    bestCycle.clearPath();
    greedy(greedyCycle);

    for (int k = 0; k < count; k++) {
        curCycle.setPath(greedyCycle);
        curCycle.shuffleSubpath(s, generator);
        localSearch(curCycle);

        if (curCycle.getCost() < bestCycle.getCost())
            bestCycle.setPath(curCycle);
    }
}

}
