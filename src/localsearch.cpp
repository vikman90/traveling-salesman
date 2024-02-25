// October 24, 2012

#include <cstdlib>
#include <climits>
#include "cycle.h"
#include "algorithms.h"

namespace Algorithms {

int localSearch(Cycle &data)
{
	const int evalsPerLoop = (int)(data.getCost() * data.getCost()) / 2;
	int evals = 0;
    bool updated;
    Cycle curCycle = data;
    Cycle &bestCycle = data;

    /*
      * We generate all possible neighbors.
      * We choose the one with the best cost.
      * We repeat until we stop getting a better neighbor.
      */

    do {
        curCycle.setPath(bestCycle);
        updated = false;

        for (int i = 1; i < data.getSize(); i++) {
            for (int j = 0; j < i; j++) {
                curCycle.swap(i, j);

                if (bestCycle.getCost() > curCycle.getCost()) {
                    bestCycle.setPath(curCycle);
                    updated = true;
                }

                curCycle.swap(i, j);
            }
        }

		evals += evalsPerLoop;
    } while (updated);

	return evals;
}

void localSearch(Cycle &data, unsigned int seed)
{
    std::mt19937 generator(seed);
    localSearch(data, generator);
}

void localSearch(Cycle &data, std::mt19937 &generator)
{
    data.shufflePath(generator);
    localSearch(data);
}

}
