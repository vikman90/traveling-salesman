/**
 * @file test_app_controller.cpp
 * @brief Integration tests for AppController API and JSON state handling
 */

#include <cassert>
#include <iostream>
#include <string>
#include "../../src/gui/app_controller.h"

int main() {
    std::cout << "[INTEGRATION TEST] Running AppController API tests..." << std::endl;

    AppController controller;

    // 1. Initial state
    auto state = controller.getGraphState();
    assert(state["status"] == "ok");
    assert(state["size"] > 0);
    assert(state["vertices"].is_array());
    assert(state["edges"].is_array());
    std::cout << "  ✓ Initial graph state verified" << std::endl;

    // 2. Map Generation modes
    auto resUniform = controller.generateRandomMap(20, 800, 600, "uniform", 101);
    assert(resUniform["status"] == "ok");
    assert(resUniform["size"] == 20);

    auto resClustered = controller.generateRandomMap(25, 800, 600, "clustered", 102);
    assert(resClustered["status"] == "ok");
    assert(resClustered["size"] == 25);

    auto resCircle = controller.generateRandomMap(30, 800, 600, "circle", 103);
    assert(resCircle["status"] == "ok");
    assert(resCircle["size"] == 30);
    std::cout << "  ✓ Map generation modes (uniform, clustered, circle) verified" << std::endl;

    // 3. Dynamic Node Editing
    auto resAdd = controller.addNode(450.0f, 350.0f);
    assert(resAdd["status"] == "ok");
    assert(resAdd["size"] == 31);

    auto resUpdate = controller.updateNodePosition(0, 100.0f, 100.0f);
    assert(resUpdate["status"] == "ok");
    std::cout << "  ✓ Dynamic node addition and coordinate editing verified" << std::endl;

    // 4. TSPLIB Preset Loading & Serialization
    auto resBerlin = controller.loadPreset("berlin52");
    assert(resBerlin["status"] == "ok");
    assert(resBerlin["size"] == 52);

    auto tspContent = controller.getTspContent();
    assert(tspContent["status"] == "ok");
    std::string tspStr = tspContent["content"];
    assert(tspStr.find("DIMENSION : 52") != std::string::npos);

    auto tourContent = controller.getTourContent();
    assert(tourContent["status"] == "ok");
    std::string tourStr = tourContent["content"];
    assert(tourStr.find("TYPE : TOUR") != std::string::npos);
    std::cout << "  ✓ Preset dataset loading and TSPLIB roundtrip verified" << std::endl;

    // 5. Algorithm Execution via JSON API
    std::vector<std::string> algos = {
        "greedy", "rs", "ls", "vnd", "sa", "greedyls", "greedyls+",
        "bmb", "grasp", "grasp+", "ils", "vns", "ga", "ma", "psa", "pga"
    };

    for (const auto &algo : algos) {
        controller.loadPreset("berlin52");
        nlohmann::json params = {
            {"algorithm", algo},
            {"count", 10},
            {"seed", 42},
            {"size", 10},
            {"processes", 3},
            {"migrLatency", 2}
        };

        auto res = controller.runAlgorithm(params);
        assert(res["status"] == "ok");
        assert(res["size"] == 52);
        assert(res["edges"].size() == 52);
        assert(res["cost"] > 0.0f);
        assert(res["timeMs"] >= 0.0);
    }
    std::cout << "  ✓ JSON Algorithm runner verified for all 16 algorithms" << std::endl;

    std::cout << "🎉 ALL APP_CONTROLLER INTEGRATION TESTS PASSED!\n" << std::endl;
    return 0;
}
