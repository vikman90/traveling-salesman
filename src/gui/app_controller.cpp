#include "app_controller.h"
#include <chrono>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>

AppController::AppController()
{
    // Initialize with a default 25-city random map
    generateRandomMap(25, 800.0f, 600.0f, "uniform", 12345);
}

nlohmann::json AppController::getGraphState()
{
    std::lock_guard<std::mutex> lock(cycleMutex);
    nlohmann::json res;
    res["status"] = "ok";
    res["size"] = cycle.getSize();
    res["cost"] = cycle.getCost();

    nlohmann::json verts = nlohmann::json::array();
    for (int i = 0; i < cycle.getSize(); i++) {
        const auto &v = cycle.vertexAt(i);
        verts.push_back({
            {"id", i + 1},
            {"x", v.x},
            {"y", v.y}
        });
    }
    res["vertices"] = verts;

    nlohmann::json edges = nlohmann::json::array();
    for (int i = 0; i < cycle.getSize(); i++) {
        edges.push_back(cycle.edgeAt(i));
    }
    res["edges"] = edges;

    return res;
}

nlohmann::json AppController::generateRandomMap(int count, float width, float height, const std::string &mode, unsigned int seed)
{
    std::lock_guard<std::mutex> lock(cycleMutex);
    if (count < 3) count = 3;
    if (count > 2000) count = 2000;
    if (width <= 100.0f) width = 800.0f;
    if (height <= 100.0f) height = 600.0f;

    std::mt19937 rng(seed == 0 ? static_cast<unsigned int>(std::time(nullptr)) : seed);
    std::vector<Cycle::Vertex> vertices(count);

    float padding = 40.0f;
    float effectiveW = width - 2 * padding;
    float effectiveH = height - 2 * padding;

    if (mode == "circle") {
        float cx = width / 2.0f;
        float cy = height / 2.0f;
        float rx = effectiveW / 2.2f;
        float ry = effectiveH / 2.2f;
        std::uniform_real_distribution<float> noiseDist(-15.0f, 15.0f);

        for (int i = 0; i < count; i++) {
            float angle = (2.0f * M_PI * i) / count;
            float rDist = noiseDist(rng);
            vertices[i].x = std::round(cx + (rx + rDist) * std::cos(angle));
            vertices[i].y = std::round(cy + (ry + rDist) * std::sin(angle));
        }
    } else if (mode == "clustered") {
        int numClusters = std::max(2, std::min(6, count / 8));
        struct Center { float x; float y; };
        std::vector<Center> centers(numClusters);
        std::uniform_real_distribution<float> cxDist(padding + 60.0f, width - padding - 60.0f);
        std::uniform_real_distribution<float> cyDist(padding + 60.0f, height - padding - 60.0f);
        for (int c = 0; c < numClusters; c++) {
            centers[c] = { cxDist(rng), cyDist(rng) };
        }

        std::uniform_int_distribution<int> clusterChoice(0, numClusters - 1);
        std::normal_distribution<float> offsetDist(0.0f, 45.0f);

        for (int i = 0; i < count; i++) {
            int c = clusterChoice(rng);
            float x = centers[c].x + offsetDist(rng);
            float y = centers[c].y + offsetDist(rng);
            x = std::max(padding, std::min(width - padding, x));
            y = std::max(padding, std::min(height - padding, y));
            vertices[i].x = std::round(x);
            vertices[i].y = std::round(y);
        }
    } else { // uniform
        std::uniform_real_distribution<float> distX(padding, width - padding);
        std::uniform_real_distribution<float> distY(padding, height - padding);
        for (int i = 0; i < count; i++) {
            vertices[i].x = std::round(distX(rng));
            vertices[i].y = std::round(distY(rng));
        }
    }

    cycle.setVertices(vertices);
    cycle.sortPath();

    nlohmann::json res;
    res["status"] = "ok";
    res["size"] = cycle.getSize();
    res["cost"] = cycle.getCost();

    nlohmann::json verts = nlohmann::json::array();
    for (int i = 0; i < cycle.getSize(); i++) {
        const auto &v = cycle.vertexAt(i);
        verts.push_back({{"id", i + 1}, {"x", v.x}, {"y", v.y}});
    }
    res["vertices"] = verts;

    nlohmann::json edges = nlohmann::json::array();
    for (int i = 0; i < cycle.getSize(); i++) {
        edges.push_back(cycle.edgeAt(i));
    }
    res["edges"] = edges;

    return res;
}

nlohmann::json AppController::loadTspString(const std::string &content)
{
    std::lock_guard<std::mutex> lock(cycleMutex);
    if (!cycle.loadTspFromString(content)) {
        return {
            {"status", "error"},
            {"message", "Failed to parse TSP format content"}
        };
    }
    cycle.sortPath();

    nlohmann::json res;
    res["status"] = "ok";
    res["size"] = cycle.getSize();
    res["cost"] = cycle.getCost();

    nlohmann::json verts = nlohmann::json::array();
    for (int i = 0; i < cycle.getSize(); i++) {
        const auto &v = cycle.vertexAt(i);
        verts.push_back({{"id", i + 1}, {"x", v.x}, {"y", v.y}});
    }
    res["vertices"] = verts;

    nlohmann::json edges = nlohmann::json::array();
    for (int i = 0; i < cycle.getSize(); i++) {
        edges.push_back(cycle.edgeAt(i));
    }
    res["edges"] = edges;

    return res;
}

nlohmann::json AppController::loadTourString(const std::string &content)
{
    std::lock_guard<std::mutex> lock(cycleMutex);
    if (!cycle.loadTourFromString(content)) {
        return {
            {"status", "error"},
            {"message", "Failed to parse TOUR format content or node dimension mismatch"}
        };
    }

    nlohmann::json res;
    res["status"] = "ok";
    res["size"] = cycle.getSize();
    res["cost"] = cycle.getCost();

    nlohmann::json edges = nlohmann::json::array();
    for (int i = 0; i < cycle.getSize(); i++) {
        edges.push_back(cycle.edgeAt(i));
    }
    res["edges"] = edges;

    return res;
}

nlohmann::json AppController::getTspContent() const
{
    std::lock_guard<std::mutex> lock(cycleMutex);
    return {
        {"status", "ok"},
        {"content", cycle.getTspString()}
    };
}

nlohmann::json AppController::getTourContent() const
{
    std::lock_guard<std::mutex> lock(cycleMutex);
    return {
        {"status", "ok"},
        {"content", cycle.getTourString()}
    };
}

nlohmann::json AppController::updateNodePosition(int index, float x, float y)
{
    std::lock_guard<std::mutex> lock(cycleMutex);
    if (index < 0 || index >= cycle.getSize()) {
        return {{"status", "error"}, {"message", "Invalid node index"}};
    }

    std::vector<Cycle::Vertex> vertices(cycle.getSize());
    for (int i = 0; i < cycle.getSize(); i++) {
        vertices[i] = cycle.vertexAt(i);
    }
    vertices[index].x = x;
    vertices[index].y = y;

    // Save existing edges
    std::vector<int> savedEdges(cycle.getSize());
    for (int i = 0; i < cycle.getSize(); i++) {
        savedEdges[i] = cycle.edgeAt(i);
    }

    cycle.setVertices(vertices);
    for (int i = 0; i < cycle.getSize(); i++) {
        cycle.edgeAt(i) = savedEdges[i];
    }
    cycle.updateCost();

    nlohmann::json res;
    res["status"] = "ok";
    res["size"] = cycle.getSize();
    res["cost"] = cycle.getCost();
    return res;
}

nlohmann::json AppController::addNode(float x, float y)
{
    std::lock_guard<std::mutex> lock(cycleMutex);
    int currentSize = cycle.getSize();
    std::vector<Cycle::Vertex> vertices(currentSize + 1);
    for (int i = 0; i < currentSize; i++) {
        vertices[i] = cycle.vertexAt(i);
    }
    vertices[currentSize].x = x;
    vertices[currentSize].y = y;

    cycle.setVertices(vertices);
    cycle.sortPath();

    nlohmann::json res;
    res["status"] = "ok";
    res["size"] = cycle.getSize();
    res["cost"] = cycle.getCost();

    nlohmann::json verts = nlohmann::json::array();
    for (int i = 0; i < cycle.getSize(); i++) {
        const auto &v = cycle.vertexAt(i);
        verts.push_back({{"id", i + 1}, {"x", v.x}, {"y", v.y}});
    }
    res["vertices"] = verts;

    nlohmann::json edges = nlohmann::json::array();
    for (int i = 0; i < cycle.getSize(); i++) {
        edges.push_back(cycle.edgeAt(i));
    }
    res["edges"] = edges;

    return res;
}

nlohmann::json AppController::clearMap()
{
    std::lock_guard<std::mutex> lock(cycleMutex);
    std::vector<Cycle::Vertex> empty;
    cycle.setVertices(empty);

    nlohmann::json res;
    res["status"] = "ok";
    res["size"] = 0;
    res["cost"] = 0.0f;
    res["vertices"] = nlohmann::json::array();
    res["edges"] = nlohmann::json::array();
    return res;
}

nlohmann::json AppController::loadPreset(const std::string &name)
{
    std::string filename = "data/" + name + ".tsp";
    std::ifstream f(filename);
    if (!f.is_open()) {
        filename = "../data/" + name + ".tsp";
        f.open(filename);
    }
    if (f.is_open()) {
        std::stringstream buffer;
        buffer << f.rdbuf();
        return loadTspString(buffer.str());
    }

    // Embedded fallback for berlin52
    if (name == "berlin52") {
        const char *berlin52 = "NAME: berlin52\nTYPE: TSP\nDIMENSION: 52\nEDGE_WEIGHT_TYPE: EUC_2D\nNODE_COORD_SECTION\n"
            "1 565.0 575.0\n2 25.0 185.0\n3 345.0 750.0\n4 945.0 685.0\n5 845.0 655.0\n6 880.0 660.0\n7 25.0 230.0\n"
            "8 525.0 1000.0\n9 580.0 1175.0\n10 650.0 1130.0\n11 1605.0 620.0\n12 1220.0 580.0\n13 1465.0 200.0\n"
            "14 1530.0 5.0\n15 845.0 680.0\n16 725.0 370.0\n17 145.0 665.0\n18 415.0 635.0\n19 510.0 875.0\n"
            "20 560.0 365.0\n21 300.0 465.0\n22 520.0 585.0\n23 480.0 415.0\n24 835.0 625.0\n25 975.0 580.0\n"
            "26 1215.0 245.0\n27 1320.0 315.0\n28 1250.0 400.0\n29 660.0 180.0\n30 410.0 250.0\n31 420.0 555.0\n"
            "32 575.0 665.0\n33 1150.0 1160.0\n34 700.0 580.0\n35 685.0 595.0\n36 685.0 610.0\n37 770.0 610.0\n"
            "38 795.0 645.0\n39 720.0 635.0\n40 760.0 650.0\n41 475.0 960.0\n42 95.0 260.0\n43 875.0 920.0\n"
            "44 700.0 500.0\n45 555.0 815.0\n46 830.0 485.0\n47 1170.0 65.0\n48 830.0 610.0\n49 605.0 625.0\n"
            "50 595.0 360.0\n51 1340.0 725.0\n52 1740.0 245.0\nEOF\n";
        return loadTspString(berlin52);
    }

    return {{"status", "error"}, {"message", "Preset file not found: " + name}};
}

nlohmann::json AppController::runAlgorithm(const nlohmann::json &params)
{
    std::lock_guard<std::mutex> lock(cycleMutex);
    if (cycle.getSize() < 3) {
        return {
            {"status", "error"},
            {"message", "The graph must contain at least 3 nodes to run algorithms"}
        };
    }

    std::string algo = params.value("algorithm", "greedy");
    int count = params.value("count", 2000);
    int size = params.value("size", 30);
    int processes = params.value("processes", 4);
    int migrLatency = params.value("migrLatency", 2);
    unsigned int seed = params.value("seed", 0);
    if (seed == 0) {
        seed = static_cast<unsigned int>(std::time(nullptr));
    }

    std::string methodStr = params.value("method", "swap");
    Algorithms::NeighborGenerator generator = (methodStr == "invert") ? Algorithms::Invert : Algorithms::Swap;

    std::string schemeStr = params.value("scheme", "gener");
    Algorithms::Scheme scheme = (schemeStr == "stat") ? Algorithms::Stationary : Algorithms::Generational;

    std::string hybStr = params.value("hybridization", "all,all");
    Algorithms::Hybridization hybridization = Algorithms::Everygen_Everychrom;
    if (hybStr == "all,2") hybridization = Algorithms::Everygen_Twochrom;
    else if (hybStr == "all,all") hybridization = Algorithms::Everygen_Everychrom;
    else if (hybStr == "10,2") hybridization = Algorithms::Tengen_Twochrom;
    else if (hybStr == "10,all") hybridization = Algorithms::Tengen_Everychrom;

    std::string topoStr = params.value("topology", "ring");
    Algorithms::Topology topology = (topoStr == "star") ? Algorithms::Star : Algorithms::Ring;

    float initialCost = cycle.getCost();
    auto start = std::chrono::high_resolution_clock::now();

    if (algo == "greedy") {
        Algorithms::greedy(cycle);
    } else if (algo == "rs") {
        Algorithms::randomSearch(cycle, count, seed);
    } else if (algo == "ls") {
        Algorithms::localSearch(cycle, seed);
    } else if (algo == "vnd") {
        Algorithms::descendantSearch(cycle, count, seed);
    } else if (algo == "sa") {
        Algorithms::simulatedAnnealing(cycle, count, seed, generator);
    } else if (algo == "greedyls") {
        Algorithms::greedy_bl(cycle);
    } else if (algo == "greedyls+") {
        Algorithms::greedy_blext(cycle, count, seed);
    } else if (algo == "bmb") {
        Algorithms::basicMultibootSearch(cycle, count, seed);
    } else if (algo == "grasp") {
        Algorithms::grasp(cycle, count, seed);
    } else if (algo == "grasp+") {
        Algorithms::graspExt(cycle, count, seed);
    } else if (algo == "ils") {
        Algorithms::iteratedLocalSearch(cycle, count, seed);
    } else if (algo == "vns") {
        Algorithms::variableSearch(cycle, count, seed);
    } else if (algo == "tabu" || algo == "ts" || algo == "taboo") {
        Algorithms::tabuSearch(cycle, count, seed);
    } else if (algo == "ga") {
        Algorithms::genetic(cycle, size, count, scheme, seed);
    } else if (algo == "ma") {
        Algorithms::memetic(cycle, size, count, hybridization, seed);
    } else if (algo == "psa") {
        Algorithms::parallelAnnealing(cycle, processes, count, migrLatency, seed);
    } else if (algo == "pga") {
        Algorithms::parallelGenetic(cycle, processes, size, count, migrLatency, topology, seed);
    } else {
        return {
            {"status", "error"},
            {"message", "Unknown algorithm: " + algo}
        };
    }

    auto end = std::chrono::high_resolution_clock::now();
    double timeMs = std::chrono::duration<double, std::milli>(end - start).count();

    nlohmann::json res;
    res["status"] = "ok";
    res["algorithm"] = algo;
    res["cost"] = cycle.getCost();
    res["initialCost"] = initialCost;
    res["timeMs"] = timeMs;
    res["seed"] = seed;
    res["size"] = cycle.getSize();

    nlohmann::json edges = nlohmann::json::array();
    for (int i = 0; i < cycle.getSize(); i++) {
        edges.push_back(cycle.edgeAt(i));
    }
    res["edges"] = edges;

    return res;
}
