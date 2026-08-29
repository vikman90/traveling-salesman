/**
 * @file test_all_engine.cpp
 * @brief Unified Engine & Algorithm Validation Test Suite
 */

#include <cassert>
#include <iomanip>
#include <iostream>
#include <vector>

#include "../src/gui/app_controller.h"

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  TSP Engine & Algorithm Validation Test" << std::endl;
    std::cout << "========================================" << std::endl;

    AppController controller;

    // Test 1: Random Map Generation
    std::cout << "\n[1] Testing Map Generation Modes..." << std::endl;
    auto resUniform = controller.generateRandomMap(25, 800, 600, "uniform", 100);
    assert(resUniform["status"] == "ok");
    assert(resUniform["size"] == 25);
    assert(resUniform["vertices"].size() == 25);
    std::cout << "  ✓ Uniform Random: 25 nodes generated (Cost: " << resUniform["cost"] << ")" << std::endl;

    auto resClustered = controller.generateRandomMap(35, 800, 600, "clustered", 200);
    assert(resClustered["status"] == "ok");
    assert(resClustered["size"] == 35);
    std::cout << "  ✓ Clustered: 35 nodes generated (Cost: " << resClustered["cost"] << ")" << std::endl;

    auto resCircle = controller.generateRandomMap(40, 800, 600, "circle", 300);
    assert(resCircle["status"] == "ok");
    assert(resCircle["size"] == 40);
    std::cout << "  ✓ Circular: 40 nodes generated (Cost: " << resCircle["cost"] << ")" << std::endl;

    // Test 2: Node Addition & Editing
    std::cout << "\n[2] Testing Dynamic Node Editing..." << std::endl;
    auto resAdd = controller.addNode(500.0f, 300.0f);
    assert(resAdd["size"] == 41);
    std::cout << "  ✓ addNode: Size increased to 41" << std::endl;

    auto resUpdate = controller.updateNodePosition(0, 150.0f, 250.0f);
    assert(resUpdate["status"] == "ok");
    std::cout << "  ✓ updateNodePosition: Node 0 updated" << std::endl;

    // Test 3: Preset Dataset Loading
    std::cout << "\n[3] Testing Preset TSPLIB Datasets..." << std::endl;
    auto resBerlin = controller.loadPreset("berlin52");
    assert(resBerlin["status"] == "ok");
    assert(resBerlin["size"] == 52);
    std::cout << "  ✓ berlin52 loaded: 52 nodes (Initial Tour Cost: " << resBerlin["cost"] << ")" << std::endl;

    // Test 4: Exporting and Round-trip Parsing
    std::cout << "\n[4] Testing TSP / TOUR Serialization Round-trip..." << std::endl;
    auto tspContent = controller.getTspContent();
    assert(tspContent["status"] == "ok");
    std::string tspStr = tspContent["content"];
    assert(tspStr.find("DIMENSION : 52") != std::string::npos);

    auto reloadRes = controller.loadTspString(tspStr);
    assert(reloadRes["status"] == "ok");
    assert(reloadRes["size"] == 52);
    std::cout << "  ✓ TSP Serialization & reload verified" << std::endl;

    // Test 5: Exhaustive Algorithm Suite Execution
    std::cout << "\n[5] Testing All 17 Heuristic and Evolutionary Algorithms..." << std::endl;

    struct AlgoTestCase {
        std::string name;
        nlohmann::json params;
    };

    std::vector<AlgoTestCase> testCases = {
        {"Greedy", {{"algorithm", "greedy"}}},
        {"Random Search (rs)", {{"algorithm", "rs"}, {"count", 100}, {"seed", 42}}},
        {"Local Search (ls)", {{"algorithm", "ls"}, {"seed", 42}}},
        {"VND (vnd)", {{"algorithm", "vnd"}, {"count", 50}, {"seed", 42}}},
        {"Simulated Annealing (sa - swap)", {{"algorithm", "sa"}, {"count", 200}, {"seed", 42}, {"method", "swap"}}},
        {"Simulated Annealing (sa - invert)", {{"algorithm", "sa"}, {"count", 200}, {"seed", 42}, {"method", "invert"}}},
        {"Greedy + LS (greedyls)", {{"algorithm", "greedyls"}}},
        {"Greedy + Extended LS (greedyls+)", {{"algorithm", "greedyls+"}, {"count", 3}, {"seed", 42}}},
        {"Basic Multiboot (bmb)", {{"algorithm", "bmb"}, {"count", 10}, {"seed", 42}}},
        {"GRASP", {{"algorithm", "grasp"}, {"count", 10}, {"seed", 42}}},
        {"GRASP Extended", {{"algorithm", "grasp+"}, {"count", 5}, {"seed", 42}}},
        {"Iterated Local Search (ils)", {{"algorithm", "ils"}, {"count", 10}, {"seed", 42}}},
        {"Variable Neighborhood Search (vns)", {{"algorithm", "vns"}, {"count", 10}, {"seed", 42}}},
        {"Tabu Search (tabu)", {{"algorithm", "tabu"}, {"count", 20}, {"seed", 42}}},
        {"Genetic Algorithm (ga - gener)", {{"algorithm", "ga"}, {"size", 10}, {"count", 50}, {"scheme", "gener"}, {"seed", 42}}},
        {"Genetic Algorithm (ga - stat)", {{"algorithm", "ga"}, {"size", 10}, {"count", 50}, {"scheme", "stat"}, {"seed", 42}}},
        {"Memetic Algorithm (ma)", {{"algorithm", "ma"}, {"size", 10}, {"count", 20}, {"hybridization", "all,2"}, {"seed", 42}}},
        {"Parallel Simulated Annealing (psa)", {{"algorithm", "psa"}, {"processes", 3}, {"count", 10}, {"migrLatency", 2}, {"seed", 42}}},
        {"Parallel Genetic (pga - ring)", {{"algorithm", "pga"}, {"processes", 3}, {"size", 10}, {"count", 20}, {"migrLatency", 2}, {"topology", "ring"}, {"seed", 42}}},
        {"Parallel Genetic (pga - star)", {{"algorithm", "pga"}, {"processes", 3}, {"size", 10}, {"count", 20}, {"migrLatency", 2}, {"topology", "star"}, {"seed", 42}}}
    };

    std::cout << std::left << std::setw(38) << "Algorithm" 
              << std::setw(16) << "Final Cost" 
              << std::setw(14) << "Time (ms)" 
              << "Status" << std::endl;
    std::cout << std::string(75, '-') << std::endl;

    for (const auto &tc : testCases) {
        controller.loadPreset("berlin52");
        auto runRes = controller.runAlgorithm(tc.params);
        assert(runRes["status"] == "ok");
        float cost = runRes["cost"];
        double timeMs = runRes["timeMs"];
        assert(cost > 0.0f);
        assert(runRes["edges"].size() == 52);

        std::cout << std::left << std::setw(38) << tc.name 
                  << std::setw(16) << cost 
                  << std::setw(14) << std::fixed << std::setprecision(2) << timeMs 
                  << "✓ PASSED" << std::endl;
    }

    std::cout << "\n🎉 ALL TESTS PASSED SUCCESSFULLY! The TSP UI engine is 100% operational." << std::endl;
    return 0;
}
