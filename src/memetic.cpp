// March 2, 2013

#include "population.h"
#include <iostream>

using namespace std;

namespace Algorithms
{

void memetic(Cycle &data, int size, int count, Hybridization hybridization, unsigned int seed)
{
	const int nMax = count * data.getSize();
	int iBest;
	int nEvolves = 0;
	int best1, best2;
	Cycle &bestCycle = data;
	mt19937 generator(seed);
	Population population(size, data, generator);

	data.setPath(population[population.bestCycle()]);

	for (int i = 0; i < nMax; i += size) {
		population.evolve(Generational);
		nEvolves++;

		switch (hybridization) {
		case Everygen_Twochrom:
			population.bestCycles(best1, best2);
			i += localSearch(population[best1]);
			i += localSearch(population[best2]);
			break;

		case Everygen_Everychrom:
			for (int j = 0; j < size; j++)
				i += localSearch(population[j]);

			break;

		case Tengen_Twochrom:
			if (nEvolves % 10 == 0) {
				population.bestCycles(best1, best2);
				i += localSearch(population[best1]);
				i += localSearch(population[best2]);
			}

			break;

		case Tengen_Everychrom:
			if (nEvolves % 10 == 0)
				for (int j = 0; j < size; j++)
					i += localSearch(population[j]);
		}

		iBest = population.bestCycle();

		if (population[iBest].getCost() < bestCycle.getCost())
			bestCycle.setPath(population[iBest]);

		#ifdef PROFILE
		cout << i << " / " << nMax << ": " << bestCycle.getCost() << endl;
		#endif
	}
}

}
