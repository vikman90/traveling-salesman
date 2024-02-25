// January 7, 2013

#include "algorithms.h"
#include "cycle.h"

namespace Algorithms
{

void basicMultibootSearch(Cycle &data, int count, unsigned int seed)
{
    Cycle &bestCycle = data;
    Cycle curCycle = data;
    std::mt19937 generator(seed);

    bestCycle.clearPath();

    for (int k = 0; k < count; k++) {
        curCycle.shufflePath(generator);
        localSearch(data);

        if (curCycle.getCost() < bestCycle.getCost())
            bestCycle.setPath(curCycle);
    }
}

}
