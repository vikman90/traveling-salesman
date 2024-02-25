// January 8, 2013

#include "cycle.h"
#include "algorithms.h"

#define KMAX 5

namespace Algorithms
{

void variableSearch(Cycle &data, int count, unsigned int seed)
{
    Cycle &bestCycle = data;
    Cycle curCycle = data;
    std::mt19937 generator(seed);
    int k = 1, bl = 0;

    bestCycle.shufflePath(generator);

    do {
        if (k > KMAX)
            k = 1;

        curCycle.setPath(bestCycle);
        curCycle.shuffleSubpath(data.getSize() / (9 - k), generator);
        localSearch(curCycle);

        if (curCycle.getCost() < bestCycle.getCost()) {
            bestCycle.setPath(curCycle);
            k = 1;
        } else
            k++;
    } while (++bl < count);
}

}
