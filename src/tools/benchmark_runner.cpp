/**
 * @file benchmark_runner.cpp
 * @brief High-Performance Automated Statistical Benchmarking Tool (tsp-benchmark)
 * @author Vikman Fernandez-Castro
 * @date Created: August 2026
 *
 * Runs the 10 official standardized seeds across TSPLIB benchmark instances
 * (berlin52, kroA100, a280) for all 18 heuristic, metaheuristic, evolutionary,
 * and parallel optimization algorithms. Calculates Best (m), Mean (x̄), Worst (p),
 * Standard Deviation (σ), Time (t), Scaled Time (t/P), and Optimal GAP (%).
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "../core/cycle.h"
#include "../algorithms/algorithms.h"

namespace {

const std::vector<unsigned int> STANDARD_SEEDS = {
    12345678,
    23456781,
    34567812,
    45678123,
    56781234,
    67812345,
    78123456,
    81234567,
    87654321,
    18765432
};

struct InstanceInfo {
    std::string name;
    std::string filename;
    int size{0};
    float optimalCost{0.0f};
};

const std::vector<InstanceInfo> BENCHMARK_INSTANCES = {
    {"berlin52", "berlin52.tsp", 52, 7542.0f},
    {"kroA100", "kroA100.tsp", 100, 21282.0f},
    {"a280", "a280.tsp", 280, 2579.0f}
};

struct AlgoConfig {
    std::string id;
    std::string displayName;
    int practice{0}; // 0 = baseline, 1 = P1, 2 = P2, 3 = P3, 4 = P4
    bool isDeterministic{false};
    int processes{1};
};

struct RunResult {
    unsigned int seed{0};
    float cost{0.0f};
    double timeSeconds{0.0};
};

struct StatsSummary {
    std::string algoId;
    std::string algoName;
    std::string instanceName;
    int practice{0};
    int processes{1};
    float optimalCost{0.0f};
    std::vector<RunResult> runs;
    float bestCost{0.0f};
    float meanCost{0.0f};
    float worstCost{0.0f};
    float stdDev{0.0f};
    double meanTimeSeconds{0.0};
    double scaledTimeSeconds{0.0};
    float gapPercent{0.0f};
};

bool resolveDataPath(const std::string &basename, std::string &resolved) {
    const std::vector<std::string> prefixes = {
        "data/" + basename,
        "../data/" + basename,
        "../../data/" + basename,
        "/home/vikman/Projects/traveling-salesman/data/" + basename
    };
    for (const auto &p : prefixes) {
        std::ifstream f(p);
        if (f.good()) {
            resolved = p;
            return true;
        }
    }
    return false;
}

RunResult executeAlgorithm(const std::string &algoId, const Cycle &initialGraph, unsigned int seed) {
    Cycle c = initialGraph;
    auto start = std::chrono::high_resolution_clock::now();

    if (algoId == "greedy") {
        Algorithms::greedy(c);
    } else if (algoId == "greedyls") {
        Algorithms::greedy_bl(c);
    } else if (algoId == "greedyls+") {
        Algorithms::greedy_blext(c, 5, seed);
    } else if (algoId == "rs") {
        Algorithms::randomSearch(c, 2000, seed);
    } else if (algoId == "ls") {
        Algorithms::localSearch(c, seed);
    } else if (algoId == "vnd") {
        Algorithms::descendantSearch(c, 2000, seed);
    } else if (algoId == "sa_swap") {
        Algorithms::simulatedAnnealing(c, 2000, seed, Algorithms::Swap);
    } else if (algoId == "sa_invert") {
        Algorithms::simulatedAnnealing(c, 2000, seed, Algorithms::Invert);
    } else if (algoId == "tabu") {
        Algorithms::tabuSearch(c, 50, seed);
    } else if (algoId == "bmb") {
        Algorithms::basicMultibootSearch(c, 50, seed);
    } else if (algoId == "grasp") {
        Algorithms::grasp(c, 50, seed);
    } else if (algoId == "grasp+") {
        Algorithms::graspExt(c, 10, seed);
    } else if (algoId == "ils") {
        Algorithms::iteratedLocalSearch(c, 50, seed);
    } else if (algoId == "vns") {
        Algorithms::variableSearch(c, 50, seed);
    } else if (algoId == "ga_gener") {
        Algorithms::genetic(c, 30, 2000, Algorithms::Generational, seed);
    } else if (algoId == "ga_stat") {
        Algorithms::genetic(c, 30, 2000, Algorithms::Stationary, seed);
    } else if (algoId == "ma_1_2") {
        Algorithms::memetic(c, 10, 2000, Algorithms::Everygen_Twochrom, seed);
    } else if (algoId == "ma_1_10") {
        Algorithms::memetic(c, 10, 2000, Algorithms::Everygen_Everychrom, seed);
    } else if (algoId == "ma_10_2") {
        Algorithms::memetic(c, 10, 2000, Algorithms::Tengen_Twochrom, seed);
    } else if (algoId == "ma_10_10") {
        Algorithms::memetic(c, 10, 2000, Algorithms::Tengen_Everychrom, seed);
    } else if (algoId == "scatter") {
        Algorithms::scatterSearch(c, 2000, seed);
    } else if (algoId == "psa") {
        Algorithms::parallelAnnealing(c, 5, 20, 1, seed);
    } else if (algoId == "pga_ring") {
        Algorithms::parallelGenetic(c, 4, 10, 50, 2, Algorithms::Ring, seed);
    } else if (algoId == "pga_star") {
        Algorithms::parallelGenetic(c, 4, 10, 50, 2, Algorithms::Star, seed);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();

    return {seed, c.getCost(), duration};
}

StatsSummary computeStats(const AlgoConfig &cfg, const InstanceInfo &inst, const std::vector<RunResult> &runs) {
    StatsSummary s;
    s.algoId = cfg.id;
    s.algoName = cfg.displayName;
    s.instanceName = inst.name;
    s.practice = cfg.practice;
    s.processes = cfg.processes;
    s.optimalCost = inst.optimalCost;
    s.runs = runs;

    if (runs.empty()) return s;

    s.bestCost = runs[0].cost;
    s.worstCost = runs[0].cost;
    double sumCost = 0.0;
    double sumTime = 0.0;

    for (const auto &r : runs) {
        if (r.cost < s.bestCost) s.bestCost = r.cost;
        if (r.cost > s.worstCost) s.worstCost = r.cost;
        sumCost += r.cost;
        sumTime += r.timeSeconds;
    }

    s.meanCost = static_cast<float>(sumCost / runs.size());
    s.meanTimeSeconds = sumTime / runs.size();
    s.scaledTimeSeconds = s.meanTimeSeconds / static_cast<double>(s.processes);

    if (runs.size() > 1) {
        double varianceSum = 0.0;
        for (const auto &r : runs) {
            double diff = r.cost - s.meanCost;
            varianceSum += diff * diff;
        }
        s.stdDev = static_cast<float>(std::sqrt(varianceSum / (runs.size() - 1)));
    } else {
        s.stdDev = 0.0f;
    }

    if (inst.optimalCost > 0.0f) {
        s.gapPercent = ((s.meanCost - inst.optimalCost) / inst.optimalCost) * 100.0f;
    }

    return s;
}

std::string formatTime(double seconds) {
    int totalSec = static_cast<int>(seconds);
    int mins = totalSec / 60;
    int secs = totalSec % 60;
    int ms = static_cast<int>((seconds - totalSec) * 1000.0);
    std::ostringstream ss;
    if (mins > 0) {
        ss << mins << "m " << secs << "s";
    } else if (secs > 0) {
        ss << secs << "." << std::setfill('0') << std::setw(2) << (ms / 10) << "s";
    } else {
        ss << ms << "ms";
    }
    return ss.str();
}

} // anonymous namespace

int main(int argc, char **argv) {
    std::cout << "================================================================================" << std::endl;
    std::cout << "  TSP High-Performance Statistical Benchmark Runner (P0 - P4 Compliance Suite)" << std::endl;
    std::cout << "================================================================================\n" << std::endl;

    int filterPractice = -1; // -1 = all
    std::string filterInstance = "all";
    std::string exportMarkdownPath = "";
    std::string exportCsvPath = "";
    bool quickMode = false;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);
        if (arg == "--practice" || arg == "-p") {
            if (++i < argc) filterPractice = std::stoi(argv[i]);
        } else if (arg == "--instance" || arg == "-i") {
            if (++i < argc) filterInstance = argv[i];
        } else if (arg == "--export" || arg == "-o" || arg == "--markdown") {
            if (++i < argc) exportMarkdownPath = argv[i];
        } else if (arg == "--csv") {
            if (++i < argc) exportCsvPath = argv[i];
        } else if (arg == "--quick" || arg == "-q") {
            quickMode = true;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: tsp-benchmark [OPTIONS]\n\n"
                      << "Options:\n"
                      << "  -p, --practice <0-4>    Filter by practical assignment (0=Greedy, 1=P1, 2=P2, 3=P3, 4=P4)\n"
                      << "  -i, --instance <name>   Filter by dataset ('berlin52', 'kroA100', 'a280', 'all')\n"
                      << "  -o, --export <file>     Export complete benchmark report as Markdown\n"
                      << "      --csv <file>        Export raw results as CSV\n"
                      << "  -q, --quick             Quick smoke mode (runs 1 seed instead of 10)\n"
                      << "  -h, --help              Show help message\n";
            return 0;
        }
    }

    const std::vector<AlgoConfig> allAlgorithms = {
        {"greedy", "Greedy Search (Deterministic)", 0, true, 1},
        {"rs", "Random Search (BA)", 1, false, 1},
        {"ls", "Local Search (BL 2-Opt)", 1, false, 1},
        {"vnd", "Variable Neighborhood Descent (VND)", 1, false, 1},
        {"sa_swap", "Simulated Annealing (ES - Swap)", 1, false, 1},
        {"sa_invert", "Simulated Annealing (ES - Invert)", 1, false, 1},
        {"tabu", "Tabu Search (BT - Edge LTM)", 1, false, 1},
        {"bmb", "Basic Multiboot (BMB)", 2, false, 1},
        {"grasp", "GRASP (Semi-Greedy LRC)", 2, false, 1},
        {"greedyls", "Greedy + Local Search", 2, true, 1},
        {"grasp+", "GRASP Extended (Hybrid)", 2, false, 1},
        {"greedyls+", "Greedy + Extended BL", 2, false, 1},
        {"ils", "Iterated Local Search (ILS)", 2, false, 1},
        {"vns", "Variable Neighborhood Search (VNS)", 2, false, 1},
        {"ga_gener", "Genetic Algorithm (Generational OX)", 3, false, 1},
        {"ga_stat", "Genetic Algorithm (Stationary OX)", 3, false, 1},
        {"ma_1_2", "Memetic Algorithm AM-(1, 2)", 3, false, 1},
        {"ma_1_10", "Memetic Algorithm AM-(1, 10)", 3, false, 1},
        {"ma_10_2", "Memetic Algorithm AM-(10, 2)", 3, false, 1},
        {"ma_10_10", "Memetic Algorithm AM-(10, 10)", 3, false, 1},
        {"scatter", "Scatter Search (Búsqueda Dispersa)", 3, false, 1},
        {"psa", "Parallel Simulated Annealing (PSA)", 4, false, 5},
        {"pga_ring", "Parallel Genetic Algorithm (PGA - Ring)", 4, false, 4},
        {"pga_star", "Parallel Genetic Algorithm (PGA - Star)", 4, false, 4}
    };

    const std::vector<unsigned int> activeSeeds = quickMode ? std::vector<unsigned int>{STANDARD_SEEDS[0]} : STANDARD_SEEDS;

    std::vector<StatsSummary> allSummaries;

    for (const auto &inst : BENCHMARK_INSTANCES) {
        if (filterInstance != "all" && inst.name != filterInstance) continue;

        std::string filePath;
        if (!resolveDataPath(inst.filename, filePath)) {
            std::cerr << "Warning: Dataset " << inst.filename << " not found. Skipping." << std::endl;
            continue;
        }

        Cycle baseGraph;
        if (!baseGraph.loadTsp(filePath)) {
            std::cerr << "Error loading TSP file: " << filePath << std::endl;
            continue;
        }

        std::cout << ">>> Running Benchmarks on Dataset: " << inst.name 
                  << " (n=" << inst.size << ", Known Optimal Cost=" << static_cast<int>(inst.optimalCost) << ")" << std::endl;
        std::cout << std::string(80, '-') << std::endl;

        for (const auto &algo : allAlgorithms) {
            if (filterPractice != -1 && algo.practice != filterPractice) continue;

            std::cout << "  • Running " << std::left << std::setw(38) << algo.displayName << " ... " << std::flush;

            std::vector<RunResult> runs;
            if (algo.isDeterministic) {
                RunResult res = executeAlgorithm(algo.id, baseGraph, 0);
                runs.push_back(res);
            } else {
                for (unsigned int seed : activeSeeds) {
                    RunResult res = executeAlgorithm(algo.id, baseGraph, seed);
                    runs.push_back(res);
                }
            }

            StatsSummary summary = computeStats(algo, inst, runs);
            allSummaries.push_back(summary);

            std::cout << "Done! Mean Cost: " << std::fixed << std::setprecision(1) << summary.meanCost
                      << " | Best: " << std::setprecision(0) << summary.bestCost
                      << " | Gap: " << std::setprecision(2) << summary.gapPercent << "%"
                      << " | Time: " << formatTime(summary.meanTimeSeconds) << std::endl;
        }
        std::cout << std::endl;
    }

    // Print Consolidated Global Comparison Markdown Table
    std::ostringstream mdOut;
    mdOut << "# 📊 TSP Algorithmic Empirical Benchmark Report (P0 - P4)\n\n"
          << "> **Standardized Protocol:** 10 independent runs per probabilistic algorithm using official seeds "
          << "(`12345678`, `23456781`, `34567812`, `45678123`, `56781234`, `67812345`, `78123456`, `81234567`, `87654321`, `18765432`). "
          << "TSPLIB rounded Euclidean distance metric.\n\n";

    mdOut << "## Global Comparative Summary Table\n\n";
    mdOut << "| Method / Algorithm | Category | Berlin52 (m / x̄ / σ / GAP%) | KroA100 (m / x̄ / σ / GAP%) | A280 (m / x̄ / σ / GAP%) | Mean Time (t / t/P) |\n";
    mdOut << "| :--- | :---: | :---: | :---: | :---: | :---: |\n";

    // Known Optimal row
    mdOut << "| **Known Optimal** | Theoretical | **7542** / **7542** / 0.0 / 0.0% | **21282** / **21282** / 0.0 / 0.0% | **2579** / **2579** / 0.0 / 0.0% | — |\n";

    // Group by algorithm
    for (const auto &algo : allAlgorithms) {
        if (filterPractice != -1 && algo.practice != filterPractice) continue;

        std::string catStr = (algo.practice == 0) ? "Baseline" : ("P" + std::to_string(algo.practice));
        mdOut << "| " << algo.displayName << " | " << catStr << " | ";

        double totalTime = 0.0;
        int instCount = 0;

        for (const auto &inst : BENCHMARK_INSTANCES) {
            auto it = std::find_if(allSummaries.begin(), allSummaries.end(), [&](const StatsSummary &s) {
                return s.algoId == algo.id && s.instanceName == inst.name;
            });

            if (it != allSummaries.end()) {
                mdOut << static_cast<int>(it->bestCost) << " / "
                      << std::fixed << std::setprecision(1) << it->meanCost << " / "
                      << std::setprecision(1) << it->stdDev << " / "
                      << std::setprecision(2) << it->gapPercent << "% | ";
                totalTime += it->meanTimeSeconds;
                instCount++;
            } else {
                mdOut << "— | ";
            }
        }

        double avgTime = (instCount > 0) ? (totalTime / instCount) : 0.0;
        double scaledTime = avgTime / algo.processes;
        mdOut << formatTime(avgTime);
        if (algo.processes > 1) {
            mdOut << " (" << formatTime(scaledTime) << ")";
        }
        mdOut << " |\n";
    }

    std::cout << "\n" << mdOut.str() << std::endl;

    if (!exportMarkdownPath.empty()) {
        std::ofstream f(exportMarkdownPath);
        if (f.is_open()) {
            f << mdOut.str();
            std::cout << "✓ Markdown report saved to: " << exportMarkdownPath << std::endl;
        }
    }

    if (!exportCsvPath.empty()) {
        std::ofstream f(exportCsvPath);
        if (f.is_open()) {
            f << "Algorithm,Practice,Dataset,Seed,Cost,TimeSeconds\n";
            for (const auto &s : allSummaries) {
                for (const auto &r : s.runs) {
                    f << s.algoId << "," << s.practice << "," << s.instanceName << ","
                      << r.seed << "," << r.cost << "," << r.timeSeconds << "\n";
                }
            }
            std::cout << "✓ Raw CSV results saved to: " << exportCsvPath << std::endl;
        }
    }

    std::cout << "🎉 ALL BENCHMARKS COMPLETED SUCCESSFULLY!\n" << std::endl;
    return 0;
}
