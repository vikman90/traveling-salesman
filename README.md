# Traveling Salesman Problem Solver & Studio

[![CMake](https://github.com/vikman90/traveling-salesman/actions/workflows/cmake.yml/badge.svg)](https://github.com/vikman90/traveling-salesman/actions/workflows/cmake.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/Standard-C%2B%2B17-emerald.svg)](https://en.cppreference.com/w/cpp/17)
[![Algorithms Guide](https://img.shields.io/badge/Docs-Algorithms%20Guide-purple.svg)](docs/ALGORITHMS.md)
[![Practicals Guide](https://img.shields.io/badge/Docs-Practicals%20P0--P4-blue.svg)](docs/PRACTICAS.md)
[![Benchmark Report](https://img.shields.io/badge/Docs-Benchmark%20Report-orange.svg)](docs/BENCHMARKS.md)
[![User Manual](https://img.shields.io/badge/Docs-User%20Manual-green.svg)](docs/USER_MANUAL.md)

A high-performance modern C++17 solution, statistical benchmarking tool, and interactive desktop visualizer for the **Traveling Salesman Problem (TSP)**. It provides 18 optimization algorithms spanning greedy heuristics, local search descent, evolutionary and genetic algorithms, scatter search, and parallel metaheuristics (fully compliant with University Metaheuristics Practicals P0 - P4).

<!-- Screenshot placeholder: replace with your image path (e.g. docs/images/screenshot.png) -->
<p align="center">
  <img src="docs/images/screenshot.png" alt="TSP Solver Studio Desktop GUI" width="850">
</p>

---

## 🏗️ Project Architecture & Modularity

The codebase is organized into cleanly decoupled modules:

```text
src/
├── core/         # Core data structures (Cycle, Population) using STL containers & Rule of Zero
├── algorithms/   # 18 heuristic, evolutionary, and parallel optimization algorithms
├── tools/        # Automated statistical benchmark runner (tsp-benchmark)
├── cli/          # High-performance CLI application (main_cli.cpp)
└── gui/          # Desktop graphical visualizer backend & controller (app_controller, main_gui)

tests/
├── unit/         # Granular unit tests (Cycle, Population, Invariants)
├── integration/  # TSPLIB benchmarks, benchmark suite & GUI AppController API tests
└── smoke/        # CLI flag matrix smoke tests

docs/
├── ALGORITHMS.md   # Comprehensive mathematical & algorithmic reference
├── PRACTICAS.md    # University metaheuristics course specification & guide (P0-P4)
├── BENCHMARKS.md   # Full 10-seed empirical statistical benchmark report
└── USER_MANUAL.md  # Complete user & developer operational manual
```

---

## 🌟 Applications Overview

The project produces three independent executables built from a unified C++17 core engine:

### 1. Desktop GUI Visualizer (`tsp-gui`)
An interactive desktop visualizer powered by native C++ and lightweight WebView (WebKitGTK on Linux, WebView2 on Windows, WKWebView on macOS):
- **Interactive 2D Canvas:** Hardware-accelerated rendering with smooth mouse-wheel zoom, pan, and real-time node dragging.
- **Dynamic City Placement:** Switch to *Add City* mode to insert custom nodes with instant path recalculation.
- **Tour Playback Animation:** Step-by-step traversal animation with glowing pulse indicator.
- **Dynamic Parameter Studio:** Forms adapt in real time to the selected algorithm (iterations, random seeds, population sizes, evolutionary schemes, migration topology).
- **Map Generator:** Generate **Uniform Random**, **Metropolitan Clusters**, and **Circular Ring** graph distributions (5 to 300+ cities).
- **TSPLIB Benchmark Presets:** Instant one-click loading of standard benchmarks (`berlin52`, `kroA100`, `a280`).
- **Drag & Drop & Export:** Drag any `.tsp` or `.tour` file onto the window, and export solutions as TSPLIB files or high-resolution PNG images.

### 2. High-Performance CLI (`tsp`)
The command-line interface for fast headless execution, scripting, and batch problem solving.

```bash
tsp [-a ALGORITHM [-n REP] [-s SEED] [-m METHOD]] [-c TOUR] [-o TOUR] TSP
```

### 3. Automated Benchmark Runner (`tsp-benchmark`)
The native statistical benchmark suite that runs the 10 official standardized seeds across TSPLIB instances and computes $m, \bar{x}, p, \sigma, t, t/P, \text{GAP}\%$.

```bash
./build/tsp-benchmark --export docs/BENCHMARKS.md --csv docs/benchmarks.csv
```

---

## 🧠 Supported Optimization Algorithms

For in-depth mathematical formulations, pseudocode, and complexity analysis, see the [Educational Algorithms Guide](docs/ALGORITHMS.md) and the [University Practicals Guide](docs/PRACTICAS.md).

| Category | Algorithm | CLI Identifier (`-a`) | Parameter Controls |
| :--- | :--- | :--- | :--- |
| **Constructive Heuristics** | Greedy Search | `greedy` | Deterministic |
| | Greedy + Local Search | `greedyls` | Deterministic |
| | Greedy + Extended Local Search | `greedyls+` | Iterations (`-n`), Seed (`-s`) |
| | GRASP (Greedy Randomized Adaptive) | `grasp` | Iterations (`-n`), Seed (`-s`) |
| | GRASP Extended | `grasp+` | Iterations (`-n`), Seed (`-s`) |
| **Neighborhood & Local Search** | Random Search | `rs` | Iterations (`-n`), Seed (`-s`) |
| | Local Search (2-Opt Best Improvement) | `ls` | Seed (`-s`) |
| | Variable Neighborhood Descent | `vnd` | Iterations (`-n`), Seed (`-s`) |
| | Basic Multiboot Search | `bmb` | Iterations (`-n`), Seed (`-s`) |
| | Iterated Local Search | `ils` | Iterations (`-n`), Seed (`-s`) |
| | Variable Neighborhood Search | `vns` | Iterations (`-n`), Seed (`-s`) |
| | Tabu Search | `tabu` | Iterations (`-n`), Seed (`-s`) |
| **Metaheuristics & Evolutionary** | Simulated Annealing | `sa` | Iterations (`-n`), Seed (`-s`), Operator (`swap`/`invert`) |
| | Genetic Algorithm | `ga` | Pop Size (`-d`), Iterations (`-n`), Scheme (`gener`/`stat`), Seed (`-s`) |
| | Memetic Algorithm | `ma` | Pop Size (`-d`), Iterations (`-n`), Hybridization Schedule, Seed (`-s`) |
| | Scatter Search | `scatter` / `bd` | Iterations (`-n`), Seed (`-s`) |
| **Parallel Metaheuristics** | Parallel Simulated Annealing | `psa` | Processes (`-p`), Latency (`-l`), Iterations (`-n`), Seed (`-s`) |
| | Parallel Genetic Algorithm | `pga` | Processes (`-p`), Latency (`-l`), Topology (`ring`/`star`), Pop Size (`-d`), Seed (`-s`) |

---

## 📦 Prerequisites & Dependencies

- **Compiler:** Modern C++17 compiler (GCC 9+, Clang 10+, MSVC 2019+)
- **CMake:** Version 3.22 or higher
- **Linux (Debian/Ubuntu packages):**
  ```bash
  sudo apt-get update && sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    libwebkit2gtk-4.1-dev \
    libgtk-3-dev \
    xvfb
  ```
  *(If building on a minimal headless Linux server without GUI libraries, CMake will automatically build the CLI and test suite).*
- **Windows:** Microsoft Edge WebView2 (included by default in Windows 10/11) and CMake 3.22+
- **macOS:** System Cocoa and WebKit frameworks (included with macOS) and CMake 3.22+

---

## 🛠️ Build & Installation

```bash
# 1. Clone repository
git clone https://github.com/vikman90/traveling-salesman.git
cd traveling-salesman

# 2. Configure with CMake
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 3. Compile all targets
cmake --build build --config Release
```

The build produces the following targets in `build/`:
- `build/tsp`: The CLI solver.
- `build/tsp-benchmark`: The automated statistical benchmark runner.
- `build/tsp-gui`: The desktop graphical application.
- `build/tsp-test`: The unified regression engine test suite.
- `build/test_cycle`, `build/test_population`, `build/test_algorithms_invariants`: Granular unit test suite.
- `build/test_tsplib_benchmarks`, `build/test_app_controller`: TSPLIB benchmark and API integration suite.
- `build/test_cli_smoke`: CLI smoke test suite.

---

## 🚀 Running the Applications

### Launch the Desktop GUI
```bash
./build/tsp-gui
```

### Run the CLI Solver
```bash
# Run Simulated Annealing on Berlin52 instance
./build/tsp -a sa -n 2000 -m invert data/berlin52.tsp

# Run Parallel Genetic Algorithm with star island topology
./build/tsp -a pga -p 4 -d 10 -n 50 -t star data/berlin52.tsp
```

### Run the Statistical Benchmark Suite
```bash
# Run all 10 standardized seeds across all datasets and export results
./build/tsp-benchmark --export docs/BENCHMARKS.md --csv docs/benchmarks.csv
```

---

## 🧪 Testing & Validation

Run all test suites with CTest:
```bash
# Run full automated test suite
ctest --test-dir build --output-on-failure
```

Or run individual target suites directly:
```bash
# Unit tests
./build/test_cycle
./build/test_population
./build/test_algorithms_invariants

# Integration tests
./build/test_tsplib_benchmarks
./build/test_app_controller

# Smoke tests
./build/test_cli_smoke

# Benchmark smoke test
./build/tsp-benchmark --quick

# Headless UI automated end-to-end simulation
xvfb-run -a ./build/tsp-gui --test-ui
```

---

## 📖 Documentation

- **[Educational Algorithms Guide (docs/ALGORITHMS.md)](docs/ALGORITHMS.md):** Theoretical foundation, pseudocode, and mathematical explanations for all 18 algorithms.
- **[University Practicals Guide (docs/PRACTICAS.md)](docs/PRACTICAS.md):** Comprehensive academic guide covering all requirements, formulas, and parameters for P0–P4.
- **[Empirical Benchmark Report (docs/BENCHMARKS.md)](docs/BENCHMARKS.md):** Complete 10-seed statistical benchmark tables across all 24 configurations.
- **[User & Developer Manual (docs/USER_MANUAL.md)](docs/USER_MANUAL.md):** End-to-end operational manual for building, running, and testing the software.
- **Doxygen API Documentation:** https://vikman90.github.io/traveling-salesman

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
