/**
 * @file test_population.cpp
 * @brief Unit tests for the Population class
 */

#include <cassert>
#include <iostream>
#include <vector>
#include <random>

#include "../../src/core/cycle.h"
#include "../../src/core/population.h"

int main() {
    std::cout << "[UNIT TEST] Running Population unit tests..." << std::endl;

    // Create a 10-city sample graph
    Cycle sample;
    std::vector<Cycle::Vertex> verts(10);
    for (int i = 0; i < 10; ++i) {
        verts[i] = {static_cast<float>(i * 20), static_cast<float>((i % 3) * 50)};
    }
    sample.setVertices(verts);

    std::mt19937 rng(12345);

    // Test 1: Population Initialization
    {
        Population pop(20, sample, rng);
        assert(pop.getSize() == 20);

        for (int i = 0; i < 20; ++i) {
            assert(pop[i].getSize() == 10);
            assert(pop[i].getCost() > 0.0f);
        }
        std::cout << "  ✓ Population constructor and size tracking verified" << std::endl;
    }

    // Test 2: Statistical Metrics (Best, Worst, Mean)
    {
        Population pop(15, sample, rng);
        int bestIdx = pop.bestCycle();
        int worstIdx = pop.worstCycle();
        float mean = pop.meanCost();

        assert(bestIdx >= 0 && bestIdx < 15);
        assert(worstIdx >= 0 && worstIdx < 15);
        assert(pop[bestIdx].getCost() <= pop[worstIdx].getCost());
        assert(mean >= pop[bestIdx].getCost());
        assert(mean <= pop[worstIdx].getCost());

        int first = -1, second = -1;
        pop.bestCycles(first, second);
        assert(first != second);
        assert(first == bestIdx);
        assert(pop[second].getCost() >= pop[first].getCost());

        std::cout << "  ✓ Population statistics (bestCycle, worstCycle, meanCost, bestCycles) verified" << std::endl;
    }

    // Test 3: Generational Evolution & Permutation Invariants
    {
        Population pop(20, sample, rng);
        for (int gen = 0; gen < 50; ++gen) {
            pop.evolve(Algorithms::Generational);
        }

        // Verify that all chromosomes in the evolved population remain valid TSP tours
        for (int p = 0; p < pop.getSize(); ++p) {
            const auto &c = pop[p];
            assert(c.getSize() == 10);
            std::vector<bool> visited(10, false);
            for (int i = 0; i < 10; ++i) {
                int node = c.edgeAt(i);
                assert(node >= 0 && node < 10);
                assert(!visited[node]);
                visited[node] = true;
            }
        }
        std::cout << "  ✓ Generational evolution invariant preservation verified" << std::endl;
    }

    // Test 4: Stationary Evolution & Permutation Invariants
    {
        Population pop(20, sample, rng);
        for (int gen = 0; gen < 100; ++gen) {
            pop.evolve(Algorithms::Stationary);
        }

        for (int p = 0; p < pop.getSize(); ++p) {
            const auto &c = pop[p];
            assert(c.getSize() == 10);
            std::vector<bool> visited(10, false);
            for (int i = 0; i < 10; ++i) {
                int node = c.edgeAt(i);
                assert(node >= 0 && node < 10);
                assert(!visited[node]);
                visited[node] = true;
            }
        }
        std::cout << "  ✓ Stationary evolution invariant preservation verified" << std::endl;
    }

    std::cout << "🎉 ALL POPULATION UNIT TESTS PASSED!\n" << std::endl;
    return 0;
}
