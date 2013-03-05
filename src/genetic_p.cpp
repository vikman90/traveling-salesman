// 3 de Marzo de 2013

#include "algorithms.h"
#include <cfloat>
#include <iostream>
#include "population.h"

using namespace std;

namespace Algorithms
{

void parallelGenetic(Cycle &data, int processes, int size, int count, int migrLatency, Topology topology, unsigned int seed)
{
	const int nMax = count * data.getSize();
	int iBest;
	float bestMean;
	std::mt19937 generator(seed);
	Cycle &bestCycle = data;
	Population *world = new Population[processes];

	for (int i = 0; i < processes; i++)
		world[i] = Population(size, data, generator);

	migrLatency *= data.getSize();

	for (int n = 0; n < nMax; n++) {
		for (int i = 0; i < processes; i++)
			world[i].evolve(Generational);

		if ((n + 1) % migrLatency == 0) {
			switch (topology) {
			case Star:
				bestMean = FLT_MAX;

				for (int i = 0; i < processes; i++) {
					float mean = world[i].meanCost();

					if (mean < bestMean) {
						iBest = i;
						bestMean = mean;
					}
				}

				for (int i = 0; i < processes; i++) {
					if (i != iBest) {
						world[i][world[i].worstCycle()].setPath(world[iBest][world[iBest].bestCycle()]);
						world[iBest][world[iBest].worstCycle()].setPath(world[i][world[i].bestCycle()]);
					}
				}

				break;

			case Ring:
				for (int i = 0; i < processes; i++)
					world[i][world[i].worstCycle()].setPath(world[(i + 1) % processes][world[(i + 1) % processes].bestCycle()]);
			}
		}

		for (int i = 0; i < processes; i++) {
			iBest = world[i].bestCycle();

			if (world[i][iBest].getCost() < bestCycle.getCost())
				bestCycle.setPath(world[i][iBest]);
		}

		#ifdef PROFILE
		cout << n << " / " << nMax << ": " << bestCycle.getCost() << endl;
		#endif
	}
}

}