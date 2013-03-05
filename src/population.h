/**
 * @file population.h
 * @brief Declaración de la clase Population
 * @date 28 de Febrero de 2012
 */

#ifndef POPULATION_H
#define POPULATION_H

#include "cycle.h"
#include "algorithms.h"

class Population
{
public:

	Population();
	explicit Population(int size, const Cycle &sample, std::mt19937 &generator);
	Population(const Population&);
	~Population();

	Population& operator =(const Population &other);

	inline Cycle& operator [](int index)
	{
		return chromosomes[index];
	}

	inline int getSize() const
	{
		return size;
	}

	int bestCycle() const;
	void bestCycles(int &iFirst, int &iSecond) const;
	int worstCycle() const;
	float meanCost() const;

	void evolve(Algorithms::Scheme scheme);

private:
	int size;
	Cycle *chromosomes;
	static std::mt19937 *generator;

	int select(int iLast) const;
	Cycle cross(int iFather, int iMother);
	void replace(int iFather, int iMother, Cycle &soon, Cycle &daughter, int iLast);
	void replace(Cycle &soon, Cycle &daughter);
	
	static bool mutateNow();
	static bool crossNow();
};

#endif