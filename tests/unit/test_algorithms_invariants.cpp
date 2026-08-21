/**
 * @file test_algorithms_invariants.cpp
 * @brief Unit tests verifying algorithmic invariants and valid permutations for all 16 TSP algorithms
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/core/cycle.h"
#include "../../src/algorithms/algorithms.h"

static void verifyPermutationInvariant(const Cycle &c, int expectedSize, const std::string &algoName) {
    assert(c.getSize() == expectedSize);
    assert(c.getCost() > 0.0f);
    assert(!std::isnan(c.getCost()));
    assert(!std::isinf(c.getCost()));

    std::vector<bool> seen(expectedSize, false);
    for (int i = 0; i < expectedSize; ++i) {
        int node = c.edgeAt(i);
        if (node < 0 || node >= expectedSize || seen[node]) {
            std::cerr << "FAILED INVARIANT in algorithm: " << algoName << " at position " << i << ", node " << node << std::endl;
            assert(false);
        }
        seen[node] = true;
    }
}

int main() {
    std::cout << "[UNIT TEST] Running Algorithms Invariants tests across all 16 algorithms..." << std::endl;

    constexpr int N = 20;
    Cycle baseGraph;
    std::vector<Cycle::Vertex> vertices(N);
    for (int i = 0; i < N; ++i) {
        float angle = (2.0f * 3.14159265f * i) / N;
        vertices[i] = {
            100.0f + 80.0f * std::cos(angle),
            100.0f + 80.0f * std::sin(angle)
        };
    }
    baseGraph.setVertices(vertices);

    // 1. Greedy
    {
        Cycle c = baseGraph;
        Algorithms::greedy(c);
        verifyPermutationInvariant(c, N, "greedy");
        std::cout << "  ✓ [1/16] greedy invariant verified" << std::endl;
    }

    // 2. Greedy + LS
    {
        Cycle c = baseGraph;
        Algorithms::greedy_bl(c);
        verifyPermutationInvariant(c, N, "greedyls");
        std::cout << "  ✓ [2/16] greedyls invariant verified" << std::endl;
    }

    // 3. Greedy + Extended LS
    {
        Cycle c = baseGraph;
        Algorithms::greedy_blext(c, 5, 42);
        verifyPermutationInvariant(c, N, "greedyls+");
        std::cout << "  ✓ [3/16] greedyls+ invariant verified" << std::endl;
    }

    // 4. Random Search
    {
        Cycle c = baseGraph;
        Algorithms::randomSearch(c, 50, 42);
        verifyPermutationInvariant(c, N, "rs");
        std::cout << "  ✓ [4/16] rs invariant verified" << std::endl;
    }

    // 5. Local Search (2-Opt)
    {
        Cycle c = baseGraph;
        Algorithms::localSearch(c, 42);
        verifyPermutationInvariant(c, N, "ls");
        std::cout << "  ✓ [5/16] ls invariant verified" << std::endl;
    }

    // 6. VND
    {
        Cycle c = baseGraph;
        Algorithms::descendantSearch(c, 20, 42);
        verifyPermutationInvariant(c, N, "vnd");
        std::cout << "  ✓ [6/16] vnd invariant verified" << std::endl;
    }

    // 7. Simulated Annealing (Swap)
    {
        Cycle c = baseGraph;
        Algorithms::simulatedAnnealing(c, 50, 42, Algorithms::Swap);
        verifyPermutationInvariant(c, N, "sa_swap");
        std::cout << "  ✓ [7/16] sa (swap) invariant verified" << std::endl;
    }

    // 8. Simulated Annealing (Invert)
    {
        Cycle c = baseGraph;
        Algorithms::simulatedAnnealing(c, 50, 42, Algorithms::Invert);
        verifyPermutationInvariant(c, N, "sa_invert");
        std::cout << "  ✓ [8/16] sa (invert) invariant verified" << std::endl;
    }

    // 9. Basic Multiboot (BMB)
    {
        Cycle c = baseGraph;
        Algorithms::basicMultibootSearch(c, 10, 42);
        verifyPermutationInvariant(c, N, "bmb");
        std::cout << "  ✓ [9/16] bmb invariant verified" << std::endl;
    }

    // 10. GRASP
    {
        Cycle c = baseGraph;
        Algorithms::grasp(c, 10, 42);
        verifyPermutationInvariant(c, N, "grasp");
        std::cout << "  ✓ [10/16] grasp invariant verified" << std::endl;
    }

    // 11. GRASP Extended
    {
        Cycle c = baseGraph;
        Algorithms::graspExt(c, 5, 42);
        verifyPermutationInvariant(c, N, "grasp+");
        std::cout << "  ✓ [11/16] grasp+ invariant verified" << std::endl;
    }

    // 12. Iterated Local Search (ILS)
    {
        Cycle c = baseGraph;
        Algorithms::iteratedLocalSearch(c, 10, 42);
        verifyPermutationInvariant(c, N, "ils");
        std::cout << "  ✓ [12/16] ils invariant verified" << std::endl;
    }

    // 13. Variable Neighborhood Search (VNS)
    {
        Cycle c = baseGraph;
        Algorithms::variableSearch(c, 10, 42);
        verifyPermutationInvariant(c, N, "vns");
        std::cout << "  ✓ [13/16] vns invariant verified" << std::endl;
    }

    // 14. Genetic Algorithm (Generational & Stationary)
    {
        Cycle cGener = baseGraph;
        Algorithms::genetic(cGener, 10, 20, Algorithms::Generational, 42);
        verifyPermutationInvariant(cGener, N, "ga_gener");

        Cycle cStat = baseGraph;
        Algorithms::genetic(cStat, 10, 20, Algorithms::Stationary, 42);
        verifyPermutationInvariant(cStat, N, "ga_stat");
        std::cout << "  ✓ [14/16] ga (gener/stat) invariants verified" << std::endl;
    }

    // 15. Memetic Algorithm
    {
        Cycle c = baseGraph;
        Algorithms::memetic(c, 10, 10, Algorithms::Everygen_Everychrom, 42);
        verifyPermutationInvariant(c, N, "ma");
        std::cout << "  ✓ [15/16] ma invariant verified" << std::endl;
    }

    // 16. Parallel Metaheuristics (PSA & PGA)
    {
        Cycle cPSA = baseGraph;
        Algorithms::parallelAnnealing(cPSA, 3, 10, 2, 42);
        verifyPermutationInvariant(cPSA, N, "psa");

        Cycle cPGA = baseGraph;
        Algorithms::parallelGenetic(cPGA, 3, 10, 10, 2, Algorithms::Ring, 42);
        verifyPermutationInvariant(cPGA, N, "pga_ring");

        Algorithms::parallelGenetic(cPGA, 3, 10, 10, 2, Algorithms::Star, 42);
        verifyPermutationInvariant(cPGA, N, "pga_star");
        std::cout << "  ✓ [16/16] psa & pga invariants verified" << std::endl;
    }

    std::cout << "🎉 ALL 16 ALGORITHM INVARIANTS PASSED!\n" << std::endl;
    return 0;
}
