# Traveling Salesman

[![CMake](https://github.com/vikman90/traveling-salesman/actions/workflows/cmake.yml/badge.svg)](https://github.com/vikman90/traveling-salesman/actions/workflows/cmake.yml)

This repository contains an application to solve the Traveling Salesman Problem (TSP) using different algorithms implemented in C++. The project was originally developed in 2013 as part of the Algorithmics practice at the University of Granada.

## Description

The [Traveling Salesman Problem](https://en.wikipedia.org/wiki/Travelling_salesman_problem) involves finding the shortest possible route that visits every city exactly once and returns to the starting point.

The application provides a range of algorithms to tackle this problem, along with additional options to customize the search and solution generation. This includes classic algorithms like Greedy Search, Local Search, and Simulated Annealing, as well as more advanced approaches such as Genetic and Memetic Algorithms.

## Usage

The application is utilized via the command line with the following syntax:

```
tsp [-a ALGORITHM [-n REP] [-s SEED] [-m METHOD]] [-c TOUR] [-o TOUR] TSP
```

For a full list of options and usage instructions, please refer to the help section within the application.

## Set up

To build and use the application, follow these steps:

1. Clone this repository to your local machine.
2. Navigate to the project directory.
3. Compile the source code using `cmake` and a C++ compiler:
```shell
cmake -B build
cmake --build build
```
4. Run the executable file with appropriate command-line arguments:
```shell
build/tsp
```

## Development documentation

https://vikman90.github.io/traveling-salesman

## Contributing

Contributions are welcome! If you'd like to contribute to this project, please fork the repository and submit a pull request with your proposed changes.

## License

This project is licensed under the [MIT License](LICENSE).
