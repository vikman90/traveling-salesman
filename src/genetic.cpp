// March 1, 2013

#include "population.h"
#include <iostream>

using namespace std;

namespace Algorithms
{

void genetic(Cycle &data, int size, int count, Scheme scheme, unsigned int seed)
{
	const int nMax = count * data.getSize();
	int iBest;
	Cycle &bestCycle = data;
	mt19937 generator(seed);
	Population population(size, data, generator);

	data.setPath(population[population.bestCycle()]);

	for (int i = 0; i < nMax; i += (scheme == Generational ? size : 2)) {
		population.evolve(scheme);
		iBest = population.bestCycle();

		if (population[iBest].getCost() < bestCycle.getCost())
			bestCycle.setPath(population[iBest]);

		#ifdef PROFILE
		cout << i << " / " << nMax << ": " << bestCycle.getCost() << endl;
		#endif
	}
}

}
