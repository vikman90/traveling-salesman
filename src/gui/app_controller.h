#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include <string>
#include <vector>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../core/cycle.h"
#include "../algorithms/algorithms.h"

class AppController {
public:
    AppController();
    ~AppController() = default;

    /// Get the current graph state (vertices, edges, cost, size)
    nlohmann::json getGraphState();

    /// Generate a new map with random/clustered/circular distribution
    nlohmann::json generateRandomMap(int count, float width, float height, const std::string &mode, unsigned int seed);

    /// Load TSP data from string (TSPLIB format)
    nlohmann::json loadTspString(const std::string &content);

    /// Load TOUR data from string (TSPLIB tour format)
    nlohmann::json loadTourString(const std::string &content);

    /// Get TSP content as TSPLIB format string
    nlohmann::json getTspContent() const;

    /// Get TOUR content as TSPLIB format string
    nlohmann::json getTourContent() const;

    /// Update coordinates of a specific node
    nlohmann::json updateNodePosition(int index, float x, float y);

    /// Add a new node at specified coordinates
    nlohmann::json addNode(float x, float y);

    /// Clear all nodes
    nlohmann::json clearMap();

    /// Run an algorithm with given parameters and return the result
    nlohmann::json runAlgorithm(const nlohmann::json &params);

    /// Load one of the built-in preset datasets ("berlin52", "kroA100", "a280")
    nlohmann::json loadPreset(const std::string &name);

private:
    Cycle cycle;
    mutable std::mutex cycleMutex;
};

#endif // APP_CONTROLLER_H
