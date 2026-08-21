/**
 * @file test_cycle.cpp
 * @brief Unit tests for the Cycle class
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>
#include <random>

#include "../../src/core/cycle.h"

int main() {
    std::cout << "[UNIT TEST] Running Cycle unit tests..." << std::endl;

    // Test 1: Default Construction
    {
        Cycle c;
        assert(c.getSize() == 0);
        assert(c.getCost() == FLT_MAX);
        assert(c.getVertices().empty());
        assert(c.getEdges().empty());
        std::cout << "  ✓ Default constructor verified" << std::endl;
    }

    // Test 2: Sized Constructor & Sequence Initialization
    {
        Cycle c(5);
        assert(c.getSize() == 5);
        assert(c.getVertices().size() == 5);
        assert(c.getEdges().size() == 5);
        for (int i = 0; i < 5; ++i) {
            assert(c.edgeAt(i) == i);
        }
        std::cout << "  ✓ Sized constructor & std::iota initialization verified" << std::endl;
    }

    // Test 3: Vertex Setup & Euclidean Distance Matrix Calculation
    {
        Cycle c;
        std::vector<Cycle::Vertex> verts = {
            {0.0f, 0.0f},   // Node 0
            {3.0f, 4.0f},   // Node 1 (Distance to 0: 5.0)
            {0.0f, 4.0f}    // Node 2 (Distance to 0: 4.0, to 1: 3.0)
        };
        c.setVertices(verts);
        assert(c.getSize() == 3);
        assert(c.distance(0, 0) == 0.0f);
        assert(c.distance(0, 1) == 5.0f);
        assert(c.distance(1, 0) == 5.0f);
        assert(c.distance(0, 2) == 4.0f);
        assert(c.distance(1, 2) == 3.0f);

        // Path is 0 -> 1 -> 2 -> 0: cost = 5 + 3 + 4 = 12
        assert(c.getCost() == 12.0f);
        std::cout << "  ✓ Set vertices and TSPLIB floor Euclidean distance calculation verified" << std::endl;
    }

    // Test 4: Copy and Move Semantics (Rule of Zero)
    {
        Cycle c1(10);
        std::vector<Cycle::Vertex> verts(10);
        for (int i = 0; i < 10; ++i) {
            verts[i] = {static_cast<float>(i * 10), static_cast<float>(i * 5)};
        }
        c1.setVertices(verts);

        // Copy constructor
        Cycle c2 = c1;
        assert(c2.getSize() == 10);
        assert(c2.getCost() == c1.getCost());
        assert(c2.vertexAt(3).x == 30.0f);

        // Move constructor
        Cycle c3 = std::move(c2);
        assert(c3.getSize() == 10);
        assert(c3.getCost() == c1.getCost());

        std::cout << "  ✓ Copy and Move semantics (Rule of Zero) verified" << std::endl;
    }

    // Test 5: Neighborhood Operators (Swap, Invert, Shuffle)
    {
        std::vector<Cycle::Vertex> verts = {
            {0.0f, 0.0f},
            {10.0f, 0.0f},
            {10.0f, 10.0f},
            {0.0f, 10.0f}
        };
        Cycle c;
        c.setVertices(verts);
        float origCost = c.getCost(); // 10 + 10 + 10 + 10 = 40

        // Swap nodes 1 and 2
        c.swap(1, 2);
        assert(c.edgeAt(1) == 2);
        assert(c.edgeAt(2) == 1);
        float swappedCost = c.getCost();

        // Swap back
        c.swap(1, 2);
        assert(c.getCost() == origCost);

        // Invert subpath
        c.invertSubpath(1, 2); // Inverts elements at index 1 and 2
        assert(c.edgeAt(1) == 2);
        assert(c.edgeAt(2) == 1);
        assert(c.getCost() == swappedCost);

        // Shuffle with seed
        std::mt19937 rng(42);
        c.shufflePath(rng);
        assert(c.getSize() == 4);
        // Verify it remains a valid permutation of {0, 1, 2, 3}
        std::vector<bool> seen(4, false);
        for (int i = 0; i < 4; ++i) {
            int node = c.edgeAt(i);
            assert(node >= 0 && node < 4);
            assert(!seen[node]);
            seen[node] = true;
        }

        std::cout << "  ✓ Swap, InvertSubpath, and ShufflePath mutations verified" << std::endl;
    }

    // Test 6: TSPLIB String Serialization & Deserialization
    {
        Cycle c;
        std::vector<Cycle::Vertex> verts = {
            {100.0f, 200.0f},
            {300.0f, 400.0f},
            {500.0f, 600.0f}
        };
        c.setVertices(verts);

        std::string tspStr = c.getTspString();
        assert(tspStr.find("TYPE : TSP") != std::string::npos);
        assert(tspStr.find("DIMENSION : 3") != std::string::npos);

        Cycle cLoaded;
        bool loaded = cLoaded.loadTspFromString(tspStr);
        assert(loaded);
        assert(cLoaded.getSize() == 3);
        assert(cLoaded.vertexAt(0).x == 100.0f);
        assert(cLoaded.vertexAt(1).y == 400.0f);
        assert(cLoaded.vertexAt(2).x == 500.0f);

        std::string tourStr = c.getTourString();
        assert(tourStr.find("TYPE : TOUR") != std::string::npos);
        assert(tourStr.find("DIMENSION : 3") != std::string::npos);

        bool tourLoaded = cLoaded.loadTourFromString(tourStr);
        assert(tourLoaded);

        std::cout << "  ✓ TSPLIB serialization and round-trip parsing verified" << std::endl;
    }

    std::cout << "🎉 ALL CYCLE UNIT TESTS PASSED!\n" << std::endl;
    return 0;
}
