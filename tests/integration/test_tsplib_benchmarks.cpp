/**
 * @file test_tsplib_benchmarks.cpp
 * @brief Integration tests running TSP algorithms on standard TSPLIB benchmark datasets
 */

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/core/cycle.h"
#include "../../src/algorithms/algorithms.h"

static bool findDataFile(const std::string &basename, std::string &foundPath) {
    std::vector<std::string> candidates = {
        "data/" + basename,
        "../data/" + basename,
        "../../data/" + basename
    };
    for (const auto &p : candidates) {
        std::ifstream f(p);
        if (f.good()) {
            foundPath = p;
            return true;
        }
    }
    return false;
}

int main() {
    std::cout << "[INTEGRATION TEST] Running TSPLIB Benchmarks..." << std::endl;

    std::string berlinPath;
    bool foundBerlin = findDataFile("berlin52.tsp", berlinPath);
    assert(foundBerlin && "berlin52.tsp must be accessible for benchmark tests");

    Cycle berlin;
    assert(berlin.loadTsp(berlinPath));
    assert(berlin.getSize() == 52);

    std::mt19937 rng(42);
    berlin.shufflePath(rng);
    float randomCost = berlin.getCost();
    std::cout << "  • berlin52 initial random tour cost: " << randomCost << std::endl;

    // Test 1: Greedy Heuristic
    {
        Cycle c = berlin;
        Algorithms::greedy(c);
        float greedyCost = c.getCost();
        std::cout << "  • berlin52 Greedy cost: " << greedyCost << std::endl;
        assert(greedyCost < randomCost * 0.5f); // Greedy is much better than random
        assert(greedyCost <= 8500.0f);
    }

    // Test 2: Local Search (2-Opt)
    {
        Cycle c = berlin;
        Algorithms::localSearch(c, 42);
        float lsCost = c.getCost();
        std::cout << "  • berlin52 2-Opt Local Search cost: " << lsCost << std::endl;
        assert(lsCost < randomCost * 0.6f);
    }

    // Test 3: Simulated Annealing (Invert move)
    {
        Cycle c = berlin;
        Algorithms::simulatedAnnealing(c, 500, 42, Algorithms::Invert);
        float saCost = c.getCost();
        std::cout << "  • berlin52 Simulated Annealing cost: " << saCost << std::endl;
        assert(saCost < randomCost * 0.6f);
    }

    // Test 4: Variable Neighborhood Search (VNS)
    {
        Cycle c = berlin;
        Algorithms::variableSearch(c, 20, 42);
        float vnsCost = c.getCost();
        std::cout << "  • berlin52 VNS cost: " << vnsCost << std::endl;
        assert(vnsCost < randomCost * 0.55f);
    }

    // Test 5: Parallel Genetic Algorithm (PGA - Ring)
    {
        Cycle c = berlin;
        Algorithms::parallelGenetic(c, 4, 20, 50, 2, Algorithms::Ring, 42);
        float pgaCost = c.getCost();
        std::cout << "  • berlin52 Parallel GA (Ring) cost: " << pgaCost << std::endl;
        assert(pgaCost < randomCost * 0.7f);
    }

    std::string kroAPath;
    if (findDataFile("kroA100.tsp", kroAPath)) {
        Cycle kroA;
        assert(kroA.loadTsp(kroAPath));
        assert(kroA.getSize() == 100);

        Cycle c = kroA;
        Algorithms::greedy(c);
        std::cout << "  • kroA100 Greedy cost: " << c.getCost() << std::endl;
        assert(c.getCost() > 0.0f);
    }

    std::cout << "🎉 ALL TSPLIB INTEGRATION TESTS PASSED!\n" << std::endl;
    return 0;
}
