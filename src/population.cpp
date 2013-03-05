/**
 * @file population.h
 * @brief Definición de la clase Population
 * @date 28 de Febrero de 2012
 */

#include "population.h"
#include <cfloat>
#include <set>

#define PROB_MUTATE 0.1
#define PROB_CROSS 0.7

std::mt19937 *Population::generator;

//------------------------------------------------------------------------------

Population::Population()
{
	size = 0;
	chromosomes = NULL;
}

//------------------------------------------------------------------------------

Population::Population(int size, const Cycle &sample, std::mt19937 &generator)
{
	this->size = size;
	chromosomes = new Cycle[size];

	for (int i = 0; i < size; i++) {
		chromosomes[i] = sample;
		chromosomes[i].shufflePath(generator);
	}

	Population::generator = &generator;
}

//------------------------------------------------------------------------------

Population::Population(const Population &other)
{
	size = other.size;
	chromosomes = new Cycle[other.size];

	for (int i = 0; i < size; i++)
		chromosomes[i] = other.chromosomes[i];
}

//------------------------------------------------------------------------------

Population::~Population()
{
	delete [] chromosomes;
}

//------------------------------------------------------------------------------

Population& Population::operator =(const Population &other)
{
	size = other.size;
	delete [] chromosomes;
	chromosomes = new Cycle[other.size];

	for (int i = 0; i < size; i++)
		chromosomes[i] = other.chromosomes[i];

	return *this;
}

//------------------------------------------------------------------------------

int Population::bestCycle() const
{
	int iBest = 0;

	for (int i = 1; i < size; i++)
		if (chromosomes[i].getCost() < chromosomes[iBest].getCost())
			iBest = i;

	return iBest;
}

//------------------------------------------------------------------------------

void Population::bestCycles(int &iFirst, int &iSecond) const
{
	iFirst = 0;

	for (int i = 1; i < size; i++)
		if (chromosomes[i].getCost() < chromosomes[iFirst].getCost())
			iFirst = i;

	iSecond = 0;

	for (int i = 1; i < size; i++)
		if (i != iFirst && chromosomes[i].getCost() < chromosomes[iFirst].getCost())
			iSecond = i;
}

//------------------------------------------------------------------------------

int Population::worstCycle() const
{
	int iWorst = 0;

	for (int i = 1; i < size; i++)
		if (chromosomes[i].getCost() > chromosomes[iWorst].getCost())
			iWorst = i;

	return iWorst;
}

//------------------------------------------------------------------------------

float Population::meanCost() const
{
	float cost = 0.0;

	for (int i = 0; i < size; i++)
		cost += chromosomes[i].getCost();

	return cost / size;
}

//------------------------------------------------------------------------------

void Population::evolve(Algorithms::Scheme scheme)
{
	int iFather;
	int iMother;
	int iWorst;
	Cycle soon;
	Cycle daughter;
	Cycle best;

	switch (scheme) {
	case Algorithms::Generational:
		best = chromosomes[bestCycle()];

		for (int iLast = size; iLast > 2; iLast -= 2) {
			iFather = select(iLast);
			
			do {
				iMother = select(iLast);
			} while (iFather == iMother);

			if (crossNow()) {
				soon = cross(iFather, iMother);
				daughter = cross(iMother, iFather);
			}
			else {
				soon.setPath(chromosomes[iFather]);
				daughter.setPath(chromosomes[iMother]);
			}

			if (mutateNow()) {
				soon.shuffleSubpath(soon.getSize() / 8, *generator);
			}

			if (mutateNow()) {
				daughter.shuffleSubpath(daughter.getSize() / 8, *generator);
			}

			replace(iFather, iMother, soon, daughter, iLast);
		}

		iWorst = worstCycle();

		if (best.getCost() < chromosomes[iWorst].getCost())
			chromosomes[iWorst].setPath(best);

		break;

	case Algorithms::Stationary:
		iFather = select(size);

		do {
			iMother = select(size);
		} while (iFather == iMother);

		soon = cross(iFather, iMother);
		daughter = cross(iMother, iFather);

		if (mutateNow()) {
			soon.shuffleSubpath(soon.getSize() / 8, *generator);
		}

		if (mutateNow()) {
			daughter.shuffleSubpath(daughter.getSize() / 8, *generator);
		}

		replace(soon, daughter);
	}
}

//------------------------------------------------------------------------------

int Population::select(int iLast) const
{
	int chrom1 = Algorithms::random(*generator, iLast);
	int chrom2 = Algorithms::random(*generator, iLast);

	return chromosomes[chrom1].getCost() < chromosomes[chrom2].getCost() ? chrom1 : chrom2;
}

//------------------------------------------------------------------------------

Cycle Population::cross(int iFather, int iMother)
{
	const int n = chromosomes->getSize();
	const Cycle &father = chromosomes[iFather];
	const Cycle &mother = chromosomes[iMother];

	int bound1 = Algorithms::random(*generator, n - 2) + 1;
	int bound2 = Algorithms::random(*generator, n - bound1 - 1) + bound1 + 1;
	int ptr = bound1;
	int edge;

	Cycle soon(chromosomes[iFather]);
	std::set<int> bounded;

	for (int i = bound1; i < bound2; i++)
		bounded.insert(father.edgeAt(i));

	for (int i = 0; i < n; i++) {
		edge = mother.edgeAt(i);

		if (bounded.count(edge))
			soon.edgeAt(ptr++) = edge;
	}

	soon.updateCost();
	return soon;
}

//------------------------------------------------------------------------------

void Population::replace(int iFather, int iMother, Cycle &soon, Cycle &daughter, int iLast)
{
	chromosomes[iFather].setPath(chromosomes[iLast - 2]);
	chromosomes[iMother].setPath(chromosomes[iLast - 1]);
	chromosomes[iLast - 2].setPath(soon);
	chromosomes[iLast - 1].setPath(daughter);
}

//------------------------------------------------------------------------------

void Population::replace(Cycle &soon, Cycle &daughter)
{
	int iWorst = worstCycle();

	if (chromosomes[iWorst].getCost() > soon.getCost())
		chromosomes[iWorst].setPath(soon);

	iWorst = worstCycle();

	if (chromosomes[iWorst].getCost() > daughter.getCost())
		chromosomes[iWorst].setPath(daughter);
}

//------------------------------------------------------------------------------

bool Population::mutateNow()
{
	return (double)(*generator)() / generator->max() <= PROB_MUTATE;
}

//------------------------------------------------------------------------------

bool Population::crossNow()
{
	return (double)(*generator)() / generator->max() <= PROB_CROSS;
}