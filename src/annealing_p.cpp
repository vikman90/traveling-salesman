// March 3, 2013

#include "algorithms.h"
#include <iostream>
#include "cycle.h"

using namespace std;

namespace Algorithms
{

void parallelAnnealing(Cycle &data, int processes, int count, int migrLatency, unsigned int seed)
{
	const int times = count / migrLatency; // (count * n) / (migrLatency * n)
	std::mt19937 generator(seed);
	Cycle &bestCycle = data;
	Cycle *cycles = new Cycle[processes];

	for (int i = 0; i < processes; i++) {
		cycles[i] = data;
		cycles[i].shufflePath(generator);
	}

	for (int k = 1; k < times; k++) {

		// Cool

		for (int i = 0; i < processes; i++)
			simulatedAnnealing(cycles[i], 1, generator, Swap);

		// Search best cycle

		for (int i = 0; i < processes; i++)
			if (cycles[i].getCost() < bestCycle.getCost())
				bestCycle.setPath(cycles[i]);

		// Update status

		if (k < times - 1)
			for (int i = 0; i < processes; i++)
				cycles[i].setPath(bestCycle);

		#ifdef PROFILE
		cout << k << " / " << times << ": " << bestCycle.getCost() << endl;
		#endif
	}

	delete [] cycles;
}

}
