/**
 * @file test_cli_smoke.cpp
 * @brief Smoke tests exercising CLI executable options and return codes
 */

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static std::string findBinary(const std::string &name) {
    std::vector<std::string> paths = {
        "./" + name,
        "./build/" + name,
        "../" + name,
        "../build/" + name
    };
    for (const auto &p : paths) {
        std::ifstream f(p);
        if (f.good()) {
            return p;
        }
    }
    return "./build/" + name;
}

static std::string findDataFile(const std::string &name) {
    std::vector<std::string> paths = {
        "data/" + name,
        "../data/" + name,
        "../../data/" + name
    };
    for (const auto &p : paths) {
        std::ifstream f(p);
        if (f.good()) {
            return p;
        }
    }
    return "data/" + name;
}

int main() {
    std::cout << "[SMOKE TEST] Running CLI Smoke Tests..." << std::endl;

    std::string tspBin = findBinary("tsp");
    std::string berlinTsp = findDataFile("berlin52.tsp");

    // Test 1: Help message
    {
        std::string cmd = tspBin + " --help > /dev/null 2>&1";
        int ret = std::system(cmd.c_str());
        // help prints usage and returns failure/exit code
        std::cout << "  ✓ CLI help invocation executed" << std::endl;
    }

    // Test 2: Invalid dataset path should return failure
    {
        std::string cmd = tspBin + " -a greedy /non/existent/file.tsp > /dev/null 2>&1";
        int ret = std::system(cmd.c_str());
        assert(ret != 0 && "CLI should fail on non-existent TSP file");
        std::cout << "  ✓ Error handling for missing input file verified" << std::endl;
    }

    // Test 3: Standard Algorithm executions
    std::vector<std::string> testArgs = {
        "-a greedy " + berlinTsp,
        "-a greedyls " + berlinTsp,
        "-a rs -n 100 -s 42 " + berlinTsp,
        "-a ls -s 42 " + berlinTsp,
        "-a vnd -n 100 -s 42 " + berlinTsp,
        "-a sa -n 200 -m swap -s 42 " + berlinTsp,
        "-a sa -n 200 -m invert -s 42 " + berlinTsp,
        "-a bmb -n 20 -s 42 " + berlinTsp,
        "-a grasp -n 20 -s 42 " + berlinTsp,
        "-a grasp+ -n 10 -s 42 " + berlinTsp,
        "-a ils -n 20 -s 42 " + berlinTsp,
        "-a vns -n 20 -s 42 " + berlinTsp,
        "-a ga -g gener -n 50 -d 20 -s 42 " + berlinTsp,
        "-a ga -g stat -n 50 -d 20 -s 42 " + berlinTsp,
        "-a ma -h all,2 -n 30 -d 10 -s 42 " + berlinTsp,
        "-a psa -p 3 -n 20 -s 42 " + berlinTsp,
        "-a pga -t ring -p 3 -d 10 -n 30 -s 42 " + berlinTsp,
        "-a pga -t star -p 3 -d 10 -n 30 -s 42 " + berlinTsp
    };

    for (const auto &args : testArgs) {
        std::string cmd = tspBin + " " + args + " > /dev/null 2>&1";
        int ret = std::system(cmd.c_str());
        if (ret != 0) {
            std::cerr << "Smoke test failed for command: " << cmd << std::endl;
            assert(false);
        }
    }
    std::cout << "  ✓ All 18 CLI command flag combinations executed successfully" << std::endl;

    // Test 4: Save and Load Tour
    {
        std::string tmpTour = "/tmp/test_smoke_berlin.tour";
        std::string cmdSave = tspBin + " -a greedy -o " + tmpTour + " " + berlinTsp + " > /dev/null 2>&1";
        int retSave = std::system(cmdSave.c_str());
        assert(retSave == 0);

        std::string cmdLoad = tspBin + " -c " + tmpTour + " " + berlinTsp + " > /dev/null 2>&1";
        int retLoad = std::system(cmdLoad.c_str());
        assert(retLoad == 0);

        std::remove(tmpTour.c_str());
        std::cout << "  ✓ CLI TOUR export (-o) and validation (-c) verified" << std::endl;
    }

    std::cout << "🎉 ALL CLI SMOKE TESTS PASSED!\n" << std::endl;
    return 0;
}
