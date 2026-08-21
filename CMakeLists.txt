cmake_minimum_required(VERSION 3.22)

project(TSP
    VERSION 1.0
    DESCRIPTION "Solution to the traveling salesman problem with different algorithms"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch webview library
FetchContent_Declare(
    webview
    GIT_REPOSITORY https://github.com/webview/webview.git
    GIT_TAG 0.12.0
)

# Fetch nlohmann_json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(webview nlohmann_json)

# Core algorithm source files (shared between CLI and GUI)
set(CORE_SOURCES
    src/annealing.cpp
    src/annealing_p.cpp
    src/bmb.cpp
    src/cycle.cpp
    src/descendantsearch.cpp
    src/genetic.cpp
    src/genetic_p.cpp
    src/grasp.cpp
    src/greedy.cpp
    src/greedybl.cpp
    src/ils.cpp
    src/localsearch.cpp
    src/memetic.cpp
    src/population.cpp
    src/randomsearch.cpp
    src/vns.cpp
)

# Target 1: CLI Application
add_executable(tsp ${CORE_SOURCES} src/main.cpp)

# Target 2: Desktop GUI Application
set(GUI_SOURCES
    ${CORE_SOURCES}
    src/gui/app_controller.cpp
    src/gui/main_gui.cpp
)

add_executable(tsp-gui ${GUI_SOURCES})
target_include_directories(tsp-gui PRIVATE src src/gui)
target_link_libraries(tsp-gui PRIVATE webview::core nlohmann_json::nlohmann_json)

if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(tsp-gui PRIVATE Threads::Threads)
endif()

# Target 3: Test Suite
set(TEST_SOURCES
    ${CORE_SOURCES}
    src/gui/app_controller.cpp
    src/test_gui_engine.cpp
)

add_executable(tsp-test ${TEST_SOURCES})
target_include_directories(tsp-test PRIVATE src src/gui)
target_link_libraries(tsp-test PRIVATE nlohmann_json::nlohmann_json)
if(UNIX AND NOT APPLE)
    target_link_libraries(tsp-test PRIVATE Threads::Threads)
endif()
