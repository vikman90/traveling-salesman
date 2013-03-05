// 23 de Octubre de 2012

#ifndef ALGORITHMS_H
#define	ALGORITHMS_H

#include <random>
#include <climits>

class Cycle;

namespace Algorithms 
{

enum NeighborGenerator { Swap, Invert };

enum Scheme { Generational, Stationary };

enum Hybridization { 
	Everygen_Twochrom,
	Everygen_Everychrom,
	Tengen_Twochrom,
	Tengen_Everychrom
};

enum Topology { Star, Ring };

// Entero en el rango [0, max[
inline int random(std::mt19937 &generator, int max)
{
    register unsigned int value;

    do {
        value = generator();
    } while (value == generator.max());

    return (int)((double)value / generator.max() * max);
}

void greedy(Cycle &data);

int localSearch(Cycle &data);
void localSearch(Cycle &data, unsigned int seed);
void localSearch(Cycle &data, std::mt19937 &generator);

void randomSearch(Cycle &data, int count, unsigned int seed);
void descendantSearch(Cycle &data, int count, unsigned int seed);

void simulatedAnnealing(Cycle &data, int count, unsigned int seed, NeighborGenerator neighbor);
void simulatedAnnealing(Cycle &data, int count, std::mt19937 &generator, NeighborGenerator neighbor);

void greedy_bl(Cycle &data);
void greedy_blext(Cycle &data, int count, unsigned int seed);

void basicMultibootSearch(Cycle &data, int count, unsigned int seed);
void grasp(Cycle &data, int count, unsigned int seed);
void graspExt(Cycle &data, int count, unsigned int seed);
void iteratedLocalSearch(Cycle &data, int count, unsigned int seed);
void variableSearch(Cycle &data, int count, unsigned int seed);

void genetic(Cycle &data, int size, int count, Scheme scheme, unsigned int seed);
void memetic(Cycle &data, int size, int count, Hybridization hybridization, unsigned int seed);

void parallelAnnealing(Cycle &data, int processes, int count, int migrLatency, unsigned int seed);
void parallelGenetic(Cycle &data, int processes, int size, int count, int migrLatency, Topology topology, unsigned int seed);
}

#endif	// ALGORITHMS_H
