/**
 * @file population.cpp
 * @brief Implementation of the Population class
 */

#include "population.h"
#include <algorithm>
#include <numeric>
#include <unordered_set>

namespace {
constexpr double PROB_MUTATE = 0.1;
constexpr double PROB_CROSS = 0.7;
}

Population::Population(int size, const Cycle &sample, std::mt19937 &generator)
    : size(size), chromosomes(size), generator(&generator) {
    for (int i = 0; i < size; ++i) {
        chromosomes[i] = sample;
        chromosomes[i].shufflePath(generator);
    }
}

int Population::bestCycle() const {
    if (chromosomes.empty()) return 0;
    auto minIt = std::min_element(chromosomes.begin(), chromosomes.end(),
        [](const Cycle &a, const Cycle &b) {
            return a.getCost() < b.getCost();
        });
    return static_cast<int>(std::distance(chromosomes.begin(), minIt));
}

void Population::bestCycles(int &iFirst, int &iSecond) const {
    iFirst = 0;
    iSecond = 0;
    if (size < 2) return;

    if (chromosomes[1].getCost() < chromosomes[0].getCost()) {
        iFirst = 1;
        iSecond = 0;
    } else {
        iFirst = 0;
        iSecond = 1;
    }

    for (int i = 2; i < size; ++i) {
        float cost = chromosomes[i].getCost();
        if (cost < chromosomes[iFirst].getCost()) {
            iSecond = iFirst;
            iFirst = i;
        } else if (cost < chromosomes[iSecond].getCost()) {
            iSecond = i;
        }
    }
}

int Population::worstCycle() const {
    if (chromosomes.empty()) return 0;
    auto maxIt = std::max_element(chromosomes.begin(), chromosomes.end(),
        [](const Cycle &a, const Cycle &b) {
            return a.getCost() < b.getCost();
        });
    return static_cast<int>(std::distance(chromosomes.begin(), maxIt));
}

float Population::meanCost() const {
    if (chromosomes.empty()) return 0.0f;
    float sum = std::accumulate(chromosomes.begin(), chromosomes.end(), 0.0f,
        [](float total, const Cycle &c) {
            return total + c.getCost();
        });
    return sum / static_cast<float>(chromosomes.size());
}

void Population::evolve(Algorithms::Scheme scheme) {
    if (size < 3 || !generator) return;

    switch (scheme) {
    case Algorithms::Generational: {
        Cycle best = chromosomes[bestCycle()];

        for (int iLast = size; iLast > 2; iLast -= 2) {
            int iFather = select(iLast);
            int iMother;
            do {
                iMother = select(iLast);
            } while (iFather == iMother);

            Cycle son;
            Cycle daughter;

            if (crossNow()) {
                son = cross(iFather, iMother);
                daughter = cross(iMother, iFather);
            } else {
                son = chromosomes[iFather];
                daughter = chromosomes[iMother];
            }

            if (mutateNow()) {
                son.shuffleSubpath(std::max(2, son.getSize() / 8), *generator);
            }
            if (mutateNow()) {
                daughter.shuffleSubpath(std::max(2, daughter.getSize() / 8), *generator);
            }

            replace(iFather, iMother, son, daughter, iLast);
        }

        int iWorst = worstCycle();
        if (best.getCost() < chromosomes[iWorst].getCost()) {
            chromosomes[iWorst].setPath(best);
        }
        break;
    }

    case Algorithms::Stationary: {
        int iFather = select(size);
        int iMother;
        do {
            iMother = select(size);
        } while (iFather == iMother);

        Cycle son = cross(iFather, iMother);
        Cycle daughter = cross(iMother, iFather);

        if (mutateNow()) {
            son.shuffleSubpath(std::max(2, son.getSize() / 8), *generator);
        }
        if (mutateNow()) {
            daughter.shuffleSubpath(std::max(2, daughter.getSize() / 8), *generator);
        }

        replace(son, daughter);
        break;
    }
    }
}

int Population::select(int iLast) const {
    int chrom1 = Algorithms::random(*generator, iLast);
    int chrom2 = Algorithms::random(*generator, iLast);
    return (chromosomes[chrom1].getCost() < chromosomes[chrom2].getCost()) ? chrom1 : chrom2;
}

Cycle Population::cross(int iFather, int iMother) {
    const int n = chromosomes[iFather].getSize();
    const Cycle &father = chromosomes[iFather];
    const Cycle &mother = chromosomes[iMother];

    int bound1 = Algorithms::random(*generator, n - 2) + 1;
    int bound2 = Algorithms::random(*generator, n - bound1 - 1) + bound1 + 1;
    int ptr = bound1;

    Cycle son = father;
    std::unordered_set<int> bounded;
    bounded.reserve(bound2 - bound1);

    for (int i = bound1; i < bound2; ++i) {
        bounded.insert(father.edgeAt(i));
    }

    for (int i = 0; i < n; ++i) {
        int edge = mother.edgeAt(i);
        if (bounded.find(edge) != bounded.end()) {
            son.edgeAt(ptr++) = edge;
        }
    }

    son.updateCost();
    return son;
}

void Population::replace(int iFather, int iMother, Cycle &son, Cycle &daughter, int iLast) {
    chromosomes[iFather].setPath(chromosomes[iLast - 2]);
    chromosomes[iMother].setPath(chromosomes[iLast - 1]);
    chromosomes[iLast - 2].setPath(son);
    chromosomes[iLast - 1].setPath(daughter);
}

void Population::replace(Cycle &son, Cycle &daughter) {
    int iWorst = worstCycle();
    if (chromosomes[iWorst].getCost() > son.getCost()) {
        chromosomes[iWorst].setPath(son);
    }

    iWorst = worstCycle();
    if (chromosomes[iWorst].getCost() > daughter.getCost()) {
        chromosomes[iWorst].setPath(daughter);
    }
}

bool Population::mutateNow() {
    std::bernoulli_distribution dist(PROB_MUTATE);
    return dist(*generator);
}

bool Population::crossNow() {
    std::bernoulli_distribution dist(PROB_CROSS);
    return dist(*generator);
}
