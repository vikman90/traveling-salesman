# 📖 Traveling Salesman Problem (TSP) — User & Developer Manual

A comprehensive operational manual for building, running, benchmarking, and interacting with the high-performance C++17 TSP solver, its automated statistical benchmark suite, and the modern desktop visualizer studio.

---

## 📑 Table of Contents
1. [Prerequisites & System Requirements](#1-prerequisites--system-requirements)
2. [Building the Project with CMake](#2-building-the-project-with-cmake)
3. [Command-Line Interface (`tsp`)](#3-command-line-interface-tsp)
   - [Basic Syntax](#basic-syntax)
   - [Algorithm Reference & Flags](#algorithm-reference--flags)
   - [Examples by Category](#examples-by-category)
4. [Statistical Benchmark Runner (`tsp-benchmark`)](#4-statistical-benchmark-runner-tsp-benchmark)
   - [Command Options & Filtering](#command-options--filtering)
   - [Exporting Reports & CSV](#exporting-reports--csv)
5. [Desktop Graphical Visualizer (`tsp-gui`)](#5-desktop-graphical-visualizer-tsp-gui)
   - [Features & Interface](#features--interface)
   - [Interactive Workflow](#interactive-workflow)
6. [Automated Test Suite (CTest)](#6-automated-test-suite-ctest)

---

## 1. Prerequisites & System Requirements

* **C++ Compiler:** Modern C++17 compliant compiler (`g++` >= 9.0, `clang++` >= 10.0, or MSVC >= 2019).
* **Build System:** `CMake` >= 3.16.
* **Make/Ninja:** `GNU Make` or `Ninja`.
* **GUI Dependencies (Optional for Desktop GUI):**
  - Linux: `libwebkit2gtk-4.0-dev` or `libwebkit2gtk-4.1-dev`, `libgtk-3-dev`.
  - Windows: Microsoft Edge WebView2 (preinstalled on Windows 10/11).
  - macOS: Cocoa / WebKit (native).
* **Git:** Version control.

---

## 2. Building the Project with CMake

Clone and configure the repository:

```bash
# 1. Create and enter build directory
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 2. Build all targets (CLI solver, benchmark tool, GUI studio, and test suites)
cmake --build build -j$(nproc)
```

The build produces three primary executables inside `build/`:
* `build/tsp`: High-performance CLI solver.
* `build/tsp-benchmark`: Native automated statistical benchmark runner.
* `build/tsp-gui`: Interactive visual studio (if WebKit/GTK libraries are installed).

---

## 3. Command-Line Interface (`tsp`)

### Basic Syntax
```bash
tsp [-a ALGORITHM [-n REP] [-s SEED] [-m METHOD]] [-c TOUR] [-o TOUR] TSP_FILE
```

### Algorithm Reference & Flags

| Flag | Parameter | Default | Description |
| :--- | :--- | :--- | :--- |
| `-a` | Algorithm ID | `greedy` | Selects the optimization algorithm. |
| `-n` | Iteration Factor | 2000 | Number of iterations / generations factor ($N = \text{REP} \cdot n$). |
| `-s` | Random Seed | `time()` | Deterministic pseudorandom seed. |
| `-m` | Move Method | `swap` | Neighborhood operator for SA (`swap` or `invert`). |
| `-d` | Population Size | 30 | Population width for GA, MA, and PGA. |
| `-g` | Scheme | `gener` | Evolutionary scheme (`gener` or `stat`). |
| `-h` | Hybridization | `all,all` | Memetic schedule (`all,2`, `all,all`, `10,2`, `10,all`). |
| `-p` | Processes | 5 / 4 | Number of concurrent processes for PSA / PGA. |
| `-l` | Latency | 1 / 2 | Migration latency factor ($M = \text{LAT} \cdot n$). |
| `-t` | Topology | `ring` | Migration topology for PGA (`ring` or `star`). |
| `-c` | Tour File | — | Evaluates and displays the cost of a `.tour` file. |
| `-o` | Tour File | — | Saves the resulting tour to a `.tour` file. |

### Complete Algorithms List

| Algorithm ID | Algorithm Name | Practical | Default Parameters |
| :--- | :--- | :---: | :--- |
| `greedy` | Nearest Neighbor Greedy | Baseline | Deterministic |
| `greedyls` | Greedy + Local Search | P2 | Deterministic |
| `greedyls+` | Greedy + Extended BL | P2 | `-n 5` |
| `rs` | Random Search | P1 | `-n 2000` |
| `ls` | Local Search (2-Opt) | P1 | Exhaustive descent |
| `vnd` | Variable Neighborhood Descent | P1 | `-n 2000` |
| `sa` | Simulated Annealing | P1 | `-n 2000 -m invert` |
| `tabu` / `ts` | Tabu Search | P1 | `-n 50` |
| `bmb` | Basic Multiboot | P2 | `-n 50` |
| `grasp` | Semi-Greedy GRASP | P2 | `-n 50` |
| `grasp+` | Extended GRASP | P2 | `-n 10` |
| `ils` | Iterated Local Search | P2 | `-n 50` |
| `vns` | Variable Neighborhood Search | P2 | `-n 50` |
| `ga` | Genetic Algorithm | P3 | `-n 2000 -d 30 -g gener` |
| `ma` | Memetic Algorithm | P3 | `-n 2000 -d 10 -h all,all` |
| `scatter` / `bd` | Scatter Search | P3 | `-n 2000` |
| `psa` | Parallel Simulated Annealing | P4 | `-p 5 -n 20 -l 1` |
| `pga` | Parallel Genetic Algorithm | P4 | `-p 4 -d 10 -n 50 -l 2 -t ring` |

### Examples by Category

```bash
# 1. Run Greedy construction on Berlin52
./build/tsp -a greedy data/berlin52.tsp

# 2. Run Simulated Annealing with 2-Opt inversion on KroA100 with seed 12345678
./build/tsp -a sa -m invert -n 2000 -s 12345678 data/kroA100.tsp

# 3. Run Memetic Algorithm with intensive hybridization (all,all) on A280
./build/tsp -a ma -h all,all -d 10 -n 2000 -s 12345678 data/a280.tsp

# 4. Run Scatter Search (Búsqueda Dispersa) on Berlin52
./build/tsp -a scatter -n 2000 -s 12345678 data/berlin52.tsp

# 5. Run Parallel GA with Star Topology on KroA100 and save output tour
./build/tsp -a pga -t star -p 4 -d 10 -n 50 -s 12345678 -o /tmp/kroA100_pga.tour data/kroA100.tsp

# 6. Verify cost of saved tour
./build/tsp -c /tmp/kroA100_pga.tour data/kroA100.tsp
```

---

## 4. Statistical Benchmark Runner (`tsp-benchmark`)

The native benchmarking tool runs the 10 official standardized seeds across all TSPLIB benchmark instances, automatically calculating $m, \bar{x}, p, \sigma, t, t/P, \text{GAP}\%$.

### Command Options & Filtering

```bash
Usage: tsp-benchmark [OPTIONS]

Options:
  -p, --practice <0-4>    Filter by practical assignment (0=Greedy, 1=P1, 2=P2, 3=P3, 4=P4)
  -i, --instance <name>   Filter by dataset ('berlin52', 'kroA100', 'a280', 'all')
  -o, --export <file>     Export complete benchmark report as Markdown
      --csv <file>        Export raw results as CSV
  -q, --quick             Quick smoke mode (runs 1 seed instead of 10)
  -h, --help              Show help message
```

### Examples

```bash
# Run full statistical suite across all 24 configurations on all datasets:
./build/tsp-benchmark --export docs/BENCHMARKS.md --csv docs/benchmarks.csv

# Run benchmarks for Practice 3 (Genetic, Memetic, Scatter Search) on Berlin52:
./build/tsp-benchmark -p 3 -i berlin52

# Fast smoke run:
./build/tsp-benchmark --quick
```

---

## 5. Desktop Graphical Visualizer (`tsp-gui`)

The modern visualizer studio provides real-time interactive exploration of TSP instances and algorithms.

```bash
./build/tsp-gui
```

### Features
* **Interactive Canvas:** Visualizes planar coordinates, city indices, and real-time Hamiltonian cycles with animated path transitions.
* **Algorithm Studio:** Dynamic configuration panels that adapt to selected algorithm requirements (mutation rates, population sizes, hybridization schedules, island topologies).
* **Live Analytics:** Instant calculation of tour cost, percentage gap to known optimal, execution duration, and vertex statistics.
* **Preset TSPLIB Loader:** Instant one-click loading of standard datasets (`berlin52`, `kroA100`, `a280`) or custom `.tsp` / `.tour` files.

---

## 6. Automated Test Suite (CTest)

The project includes an automated test harness covering unit tests, integration benchmarks, CLI smoke tests, and regression engines:

```bash
# Run all 8 test suites
ctest --test-dir build --output-on-failure
```

```text
1/8 Test #1: Unit_Cycle ....................... Passed (0.00s)
2/8 Test #2: Unit_Population .................. Passed (0.00s)
3/8 Test #3: Unit_AlgorithmsInvariants ........ Passed (0.01s)
4/8 Test #4: Integration_TSPLIB ............... Passed (0.00s)
5/8 Test #5: Integration_AppController ........ Passed (0.06s)
6/8 Test #6: Smoke_CLI ........................ Passed (0.15s)
7/8 Test #7: Regression_EngineSuite ........... Passed (0.07s)
8/8 Test #8: Integration_BenchmarkSuite ....... Passed (2.10s)
```

