// October 23, 2012

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
     * We generate a random solution (curCycle).
     * If it improves the best candidate, we keep it.
     */

    bestCycle.shufflePath(generator);

    for (int i = 1; i < MAX; i++) {
        curCycle.shufflePath(generator);

        if (bestCycle.getCost() > curCycle.getCost())
            bestCycle.setPath(curCycle);
    }
}
}
