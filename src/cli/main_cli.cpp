/**
 * @file main_cli.cpp
 * @brief Command Line Interface (CLI) for the Traveling Salesman Problem solver
 * @author Vikman Fernandez-Castro
 * @date Created: October 22, 2012
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "../core/cycle.h"
#include "../algorithms/algorithms.h"

namespace {

enum class Algorithm {
    None, Greedy, RS, LS, VND, SA,
    GreedyLS, GreedyLSExt, BMB, Grasp, GraspExt, ILS, VNS,
    Genetic, Memetic, ParallelSA, ParallelGenetic
};

struct CliConfig {
    Algorithm algorithm{Algorithm::None};
    std::string pathTSP;
    std::string pathTourIn;
    std::string pathTourOut;
    int count{2000};
    int size{30};
    int processes{5};
    int migrLatency{1};
    unsigned int seed{0};
    Algorithms::NeighborGenerator neighborGen{Algorithms::Swap};
    Algorithms::Scheme scheme{Algorithms::Generational};
    Algorithms::Hybridization hybridization{Algorithms::Everygen_Everychrom};
    Algorithms::Topology topology{Algorithms::Ring};
};

void printHelp() {
    std::cout << "      Traveling Salesman Problem - High Performance CLI\n\n"
              << "How to use:\n"
              << "  tsp [-a ALGORITHM [-n REP] [-s SEED] [-m METHOD]] [-c TOUR] [-o TOUR] TSP\n\n"
              << "Load the TSP file and calculate the cost of a minimum route from a\n"
              << "TOUR file or based on an ALGORITHM implemented in the application.\n\n"
              << "OPTIONS:\n"
              << "  -a    Apply an ALGORITHM to calculate the cost (overrides -c).\n"
              << "  -n    Perform the algorithm (REP*n) times or generations.\n"
              << "  -s    Set the SEED of the pseudorandom generator (Default: time()).\n"
              << "  -m    Use the Neighbor Generation METHOD for Simulated Annealing.\n"
              << "  -c    Show the cost of the tour saved in the TOUR file.\n"
              << "  -o    Save the route to a TOUR file instead of displaying it.\n"
              << "  -d    Set the width/size of the population.\n"
              << "  -g    Use the evolutionary SCHEME for genetic algorithms.\n"
              << "  -h    Select the HYBRIDIZATION type for memetic algorithms.\n"
              << "  -p    Specify the number of parallel processes.\n"
              << "  -l    Change the migration latency.\n"
              << "  -t    Follow a TOPOLOGY in the parallel genetic algorithm.\n\n"
              << "ALGORITHMS:\n"
              << "  greedy    Greedy search\n"
              << "  rs        Random search\n"
              << "  ls        Local search\n"
              << "  vnd       Variable neighborhood descent search\n"
              << "  sa        Simulated annealing\n"
              << "  greedyls  Greedy + Local search\n"
              << "  greedyls+ Greedy + Extended local search\n"
              << "  bmb       Basic multiboot search\n"
              << "  grasp     Greedy randomized adaptive search\n"
              << "  grasp+    Extended GRASP\n"
              << "  ils       Iterated local search\n"
              << "  vns       Variable neighborhood search\n"
              << "  ga        Genetic algorithms\n"
              << "  ma        Memetic algorithms\n"
              << "  psa       Parallel simulated annealing\n"
              << "  pga       Parallel genetic algorithms\n\n"
              << "METHODS:\n"
              << "  swap      Swap [default]\n"
              << "  invert    Reverse subpath\n\n"
              << "EVOLUTIONARY SCHEMES:\n"
              << "  gener     Generational [default]\n"
              << "  stat      Stationary\n\n"
              << "TYPES OF HYBRIDIZATION:\n"
              << "  all,2     All generations, on the 2 best chromosomes\n"
              << "  all,all   All generations, over all chromosomes [default]\n"
              << "  10,2      Every 10 generations, on the 2 best chromosomes\n"
              << "  10,all    Every 10 generations, on all chromosomes\n\n"
              << "TOPOLOGY MODELS:\n"
              << "  star      Star\n"
              << "  ring      Ring [default]\n";
}

bool parseArgs(int argc, char **argv, CliConfig &cfg) {
    if (argc < 2) return false;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);

        if (arg == "--help" || arg == "-help") {
            return false;
        }

        if (arg == "-a") {
            if (++i >= argc) return false;
            std::string_view a(argv[i]);
            if (a == "greedy") { cfg.algorithm = Algorithm::Greedy; }
            else if (a == "rs") { cfg.algorithm = Algorithm::RS; cfg.count = 2000; }
            else if (a == "ls") { cfg.algorithm = Algorithm::LS; }
            else if (a == "vnd") { cfg.algorithm = Algorithm::VND; cfg.count = 2000; }
            else if (a == "sa") { cfg.algorithm = Algorithm::SA; cfg.count = 2000; }
            else if (a == "greedyls") { cfg.algorithm = Algorithm::GreedyLS; }
            else if (a == "greedyls+") { cfg.algorithm = Algorithm::GreedyLSExt; cfg.count = 5; }
            else if (a == "bmb") { cfg.algorithm = Algorithm::BMB; cfg.count = 50; }
            else if (a == "grasp") { cfg.algorithm = Algorithm::Grasp; cfg.count = 50; }
            else if (a == "grasp+") { cfg.algorithm = Algorithm::GraspExt; cfg.count = 10; }
            else if (a == "ils") { cfg.algorithm = Algorithm::ILS; cfg.count = 50; }
            else if (a == "vns") { cfg.algorithm = Algorithm::VNS; cfg.count = 50; }
            else if (a == "ga") { cfg.algorithm = Algorithm::Genetic; cfg.count = 2000; cfg.size = 30; }
            else if (a == "ma") { cfg.algorithm = Algorithm::Memetic; cfg.count = 2000; cfg.size = 10; }
            else if (a == "psa") { cfg.algorithm = Algorithm::ParallelSA; cfg.count = 20; cfg.processes = 5; }
            else if (a == "pga") { cfg.algorithm = Algorithm::ParallelGenetic; cfg.count = 50; cfg.size = 10; cfg.processes = 4; cfg.migrLatency = 2; }
            else return false;
        } else if (arg == "-n") {
            if (++i >= argc) return false;
            cfg.count = std::stoi(argv[i]);
        } else if (arg == "-s") {
            if (++i >= argc) return false;
            cfg.seed = static_cast<unsigned int>(std::stoul(argv[i]));
        } else if (arg == "-d") {
            if (++i >= argc) return false;
            cfg.size = std::stoi(argv[i]);
        } else if (arg == "-p") {
            if (++i >= argc) return false;
            cfg.processes = std::stoi(argv[i]);
        } else if (arg == "-l") {
            if (++i >= argc) return false;
            cfg.migrLatency = std::stoi(argv[i]);
        } else if (arg == "-m") {
            if (++i >= argc) return false;
            std::string_view m(argv[i]);
            if (m == "swap") cfg.neighborGen = Algorithms::Swap;
            else if (m == "invert") cfg.neighborGen = Algorithms::Invert;
            else return false;
        } else if (arg == "-c") {
            if (++i >= argc) return false;
            cfg.pathTourIn = argv[i];
        } else if (arg == "-o") {
            if (++i >= argc) return false;
            cfg.pathTourOut = argv[i];
        } else if (arg == "-g") {
            if (++i >= argc) return false;
            std::string_view g(argv[i]);
            if (g == "gener") cfg.scheme = Algorithms::Generational;
            else if (g == "stat") cfg.scheme = Algorithms::Stationary;
            else return false;
        } else if (arg == "-h") {
            if (++i >= argc) return false;
            std::string_view h(argv[i]);
            if (h == "all,2") cfg.hybridization = Algorithms::Everygen_Twochrom;
            else if (h == "all,all") cfg.hybridization = Algorithms::Everygen_Everychrom;
            else if (h == "10,2") cfg.hybridization = Algorithms::Tengen_Twochrom;
            else if (h == "10,all") cfg.hybridization = Algorithms::Tengen_Everychrom;
            else return false;
        } else if (arg == "-t") {
            if (++i >= argc) return false;
            std::string_view t(argv[i]);
            if (t == "star") cfg.topology = Algorithms::Star;
            else if (t == "ring") cfg.topology = Algorithms::Ring;
            else return false;
        } else if (i == argc - 1) {
            cfg.pathTSP = argv[i];
        } else {
            return false;
        }
    }

    return !cfg.pathTSP.empty();
}

} // anonymous namespace

int main(int argc, char **argv) {
    CliConfig cfg;
    cfg.seed = static_cast<unsigned int>(std::time(nullptr));

    if (!parseArgs(argc, argv, cfg)) {
        printHelp();
        return EXIT_FAILURE;
    }

    Cycle data;
    if (!data.loadTsp(cfg.pathTSP)) {
        std::cerr << "Error: Failed to load TSP dataset from: " << cfg.pathTSP << "\n";
        return EXIT_FAILURE;
    }

    auto start = std::chrono::high_resolution_clock::now();

    switch (cfg.algorithm) {
    case Algorithm::None:
        if (!cfg.pathTourIn.empty()) {
            if (!data.loadTour(cfg.pathTourIn)) {
                std::cerr << "Error: Failed to load TOUR file: " << cfg.pathTourIn << "\n";
                return EXIT_FAILURE;
            }
        } else {
            std::cerr << "Nothing to do... Please specify an algorithm (-a) or tour file (-c).\n";
            return EXIT_FAILURE;
        }
        break;

    case Algorithm::Greedy:
        Algorithms::greedy(data);
        break;

    case Algorithm::RS:
        Algorithms::randomSearch(data, cfg.count, cfg.seed);
        break;

    case Algorithm::LS:
        Algorithms::localSearch(data, cfg.seed);
        break;

    case Algorithm::VND:
        Algorithms::descendantSearch(data, cfg.count, cfg.seed);
        break;

    case Algorithm::SA:
        Algorithms::simulatedAnnealing(data, cfg.count, cfg.seed, cfg.neighborGen);
        break;

    case Algorithm::GreedyLS:
        Algorithms::greedy_bl(data);
        break;

    case Algorithm::GreedyLSExt:
        Algorithms::greedy_blext(data, cfg.count, cfg.seed);
        break;

    case Algorithm::BMB:
        Algorithms::basicMultibootSearch(data, cfg.count, cfg.seed);
        break;

    case Algorithm::Grasp:
        Algorithms::grasp(data, cfg.count, cfg.seed);
        break;

    case Algorithm::GraspExt:
        Algorithms::graspExt(data, cfg.count, cfg.seed);
        break;

    case Algorithm::ILS:
        Algorithms::iteratedLocalSearch(data, cfg.count, cfg.seed);
        break;

    case Algorithm::VNS:
        Algorithms::variableSearch(data, cfg.count, cfg.seed);
        break;

    case Algorithm::Genetic:
        Algorithms::genetic(data, cfg.size, cfg.count, cfg.scheme, cfg.seed);
        break;

    case Algorithm::Memetic:
        Algorithms::memetic(data, cfg.size, cfg.count, cfg.hybridization, cfg.seed);
        break;

    case Algorithm::ParallelSA:
        Algorithms::parallelAnnealing(data, cfg.processes, cfg.count, cfg.migrLatency, cfg.seed);
        break;

    case Algorithm::ParallelGenetic:
        Algorithms::parallelGenetic(data, cfg.processes, cfg.size, cfg.count, cfg.migrLatency, cfg.topology, cfg.seed);
        break;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double totalSeconds = std::chrono::duration<double>(end - start).count();
    int seconds = static_cast<int>(totalSeconds);
    int minutes = seconds / 60;
    seconds %= 60;

    if (!cfg.pathTourOut.empty()) {
        if (!data.saveTour(cfg.pathTourOut)) {
            std::cerr << "Error: Could not save tour output to: " << cfg.pathTourOut << "\n";
            return EXIT_FAILURE;
        }
    } else {
        std::cout << "Path:\n";
        for (int i = 0; i < data.getSize() - 1; ++i) {
            std::cout << (data.edgeAt(i) + 1) << ", ";
        }
        if (data.getSize() > 0) {
            std::cout << (data.edgeAt(data.getSize() - 1) + 1);
        }
        std::cout << "\n";
    }

    std::cout << cfg.seed << "\t" << data.getCost() << "\t"
              << std::setfill('0') << std::setw(2) << minutes << ":"
              << std::setfill('0') << std::setw(2) << seconds << "\n";

    return EXIT_SUCCESS;
}
