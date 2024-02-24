# February 24, 2024

cmake_minimum_required(VERSION 3.22)

project(TSP
    VERSION 1.0
    DESCRIPTION "Solution to the traveling salesman problem with different algorithms"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES "src/*.cpp")

add_executable(tsp ${SOURCES})
