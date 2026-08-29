cmake_minimum_required(VERSION 3.22)

project(TSP
    VERSION 2.0
    DESCRIPTION "High-performance Solution to the Traveling Salesman Problem with Modern C++"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_GUI "Build desktop GUI visualizer" ON)
option(BUILD_TESTS "Build test suites" ON)

include(FetchContent)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

# Fetch nlohmann_json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Check for GUI system dependencies
set(CAN_BUILD_GUI OFF)
if(BUILD_GUI)
    if(UNIX AND NOT APPLE)
        find_package(PkgConfig QUIET)
        if(PKG_CONFIG_FOUND)
            pkg_search_module(WEBKIT_GTK QUIET webkit2gtk-4.1 webkit2gtk-4.0 webkitgtk-6.0)
            if(WEBKIT_GTK_FOUND)
                set(CAN_BUILD_GUI ON)
            else()
                message(WARNING "WebKitGTK development headers not found. Skipping 'tsp-gui' build. Install 'libwebkit2gtk-4.1-dev' and 'libgtk-3-dev' to enable GUI.")
            endif()
        else()
            message(WARNING "pkg-config not found. Skipping 'tsp-gui' build.")
        endif()
    else()
        set(CAN_BUILD_GUI ON)
    endif()
endif()

if(CAN_BUILD_GUI)
    FetchContent_Declare(
        webview
        GIT_REPOSITORY https://github.com/webview/webview.git
        GIT_TAG 0.12.0
    )
    FetchContent_MakeAvailable(webview)
endif()

# ------------------------------------------------------------------------------
# Core & Algorithm Libraries
# ------------------------------------------------------------------------------

# 1. tsp_core library
set(CORE_SOURCES
    src/core/cycle.cpp
    src/core/population.cpp
)

add_library(tsp_core STATIC ${CORE_SOURCES})
target_include_directories(tsp_core PUBLIC src)

# 2. tsp_algorithms library
set(ALGORITHM_SOURCES
    src/algorithms/annealing.cpp
    src/algorithms/annealing_p.cpp
    src/algorithms/bmb.cpp
    src/algorithms/descendantsearch.cpp
    src/algorithms/genetic.cpp
    src/algorithms/genetic_p.cpp
    src/algorithms/grasp.cpp
    src/algorithms/greedy.cpp
    src/algorithms/greedybl.cpp
    src/algorithms/ils.cpp
    src/algorithms/localsearch.cpp
    src/algorithms/memetic.cpp
    src/algorithms/randomsearch.cpp
    src/algorithms/scatter.cpp
    src/algorithms/tabusearch.cpp
    src/algorithms/vns.cpp
)

add_library(tsp_algorithms STATIC ${ALGORITHM_SOURCES})
target_include_directories(tsp_algorithms PUBLIC src)
target_link_libraries(tsp_algorithms PUBLIC tsp_core)

# 3. tsp_gui_lib library (Controller & data layer)
add_library(tsp_gui_lib STATIC src/gui/app_controller.cpp)
target_include_directories(tsp_gui_lib PUBLIC src src/gui)
target_link_libraries(tsp_gui_lib PUBLIC tsp_algorithms nlohmann_json::nlohmann_json)

# ------------------------------------------------------------------------------
# Executable Targets
# ------------------------------------------------------------------------------

# Target 1: CLI Application
add_executable(tsp src/cli/main_cli.cpp)
target_include_directories(tsp PRIVATE src)
target_link_libraries(tsp PRIVATE tsp_algorithms)

# Target 2: Desktop GUI Application (when supported)
if(CAN_BUILD_GUI)
    add_executable(tsp-gui src/gui/main_gui.cpp)
    target_include_directories(tsp-gui PRIVATE src src/gui)
    target_link_libraries(tsp-gui PRIVATE tsp_gui_lib webview::core)

    if(UNIX AND NOT APPLE)
        find_package(Threads REQUIRED)
        target_link_libraries(tsp-gui PRIVATE Threads::Threads)
    endif()
endif()

# Target 3: Automated Statistical Benchmark Runner
add_executable(tsp-benchmark src/tools/benchmark_runner.cpp)
target_include_directories(tsp-benchmark PRIVATE src)
target_link_libraries(tsp-benchmark PRIVATE tsp_algorithms)

# ------------------------------------------------------------------------------
# Testing Scaffolding (CTest)
# ------------------------------------------------------------------------------

if(BUILD_TESTS)
    enable_testing()

    # Unit Tests: Cycle
    add_executable(test_cycle tests/unit/test_cycle.cpp)
    target_include_directories(test_cycle PRIVATE src)
    target_link_libraries(test_cycle PRIVATE tsp_core)
    add_test(NAME Unit_Cycle COMMAND test_cycle)

    # Unit Tests: Population
    add_executable(test_population tests/unit/test_population.cpp)
    target_include_directories(test_population PRIVATE src)
    target_link_libraries(test_population PRIVATE tsp_algorithms)
    add_test(NAME Unit_Population COMMAND test_population)

    # Unit Tests: Algorithmic Invariants
    add_executable(test_algorithms_invariants tests/unit/test_algorithms_invariants.cpp)
    target_include_directories(test_algorithms_invariants PRIVATE src)
    target_link_libraries(test_algorithms_invariants PRIVATE tsp_algorithms)
    add_test(NAME Unit_AlgorithmsInvariants COMMAND test_algorithms_invariants)

    # Integration Tests: TSPLIB Benchmarks
    add_executable(test_tsplib_benchmarks tests/integration/test_tsplib_benchmarks.cpp)
    target_include_directories(test_tsplib_benchmarks PRIVATE src)
    target_link_libraries(test_tsplib_benchmarks PRIVATE tsp_algorithms)
    add_test(NAME Integration_TSPLIB COMMAND test_tsplib_benchmarks)

    # Integration Tests: AppController API
    add_executable(test_app_controller tests/integration/test_app_controller.cpp)
    target_include_directories(test_app_controller PRIVATE src src/gui)
    target_link_libraries(test_app_controller PRIVATE tsp_gui_lib)
    add_test(NAME Integration_AppController COMMAND test_app_controller)

    # Smoke Tests: CLI
    add_executable(test_cli_smoke tests/smoke/test_cli_smoke.cpp)
    target_include_directories(test_cli_smoke PRIVATE src)
    add_test(NAME Smoke_CLI COMMAND test_cli_smoke)
    add_dependencies(test_cli_smoke tsp)

    # Unified Regression Test Runner (Backwards compatibility)
    add_executable(tsp-test tests/test_all_engine.cpp)
    target_include_directories(tsp-test PRIVATE src src/gui)
    target_link_libraries(tsp-test PRIVATE tsp_gui_lib)
    if(UNIX AND NOT APPLE)
        find_package(Threads REQUIRED)
        target_link_libraries(tsp-test PRIVATE Threads::Threads)
    endif()
    add_test(NAME Regression_EngineSuite COMMAND tsp-test)

    # Integration Tests: Automated Benchmark Runner Smoke
    add_test(NAME Integration_BenchmarkSuite COMMAND tsp-benchmark --quick)
endif()
