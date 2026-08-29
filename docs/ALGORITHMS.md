# 📘 Traveling Salesman Problem (TSP) — Educational Algorithm Guide

An exhaustive pedagogical and mathematical guide to the **Traveling Salesman Problem (TSP)** and the 18 heuristic, metaheuristic, evolutionary, and parallel algorithms implemented in this project (fully aligned with university practicals P0, P1, P2, P3, and P4).

---

## 📑 Table of Contents
1. [Mathematical Formulation & Complexity](#1-mathematical-formulation--complexity)
2. [Taxonomy of Implemented Algorithms](#2-taxonomy-of-implemented-algorithms)
3. [Category 1: Constructive Heuristics](#3-category-1-constructive-heuristics)
   - [Greedy Search (Nearest Neighbor)](#31-greedy-search-greedy)
   - [Greedy + Local Search](#32-greedy--local-search-greedyls)
   - [Greedy + Extended Local Search](#33-greedy--extended-local-search-greedyls)
   - [GRASP (Greedy Randomized Adaptive Search)](#34-grasp-grasp)
   - [GRASP Extended](#35-grasp-extended-grasp)
4. [Category 2: Neighborhood & Local Search](#4-category-2-neighborhood--local-search)
   - [Random Search](#41-random-search-rs)
   - [Local Search (2-Opt Best Improvement)](#42-local-search-2-opt-ls)
   - [Variable Neighborhood Descent (VND)](#43-variable-neighborhood-descent-vnd)
   - [Basic Multiboot Search (BMB)](#44-basic-multiboot-search-bmb)
   - [Iterated Local Search (ILS)](#45-iterated-local-search-ils)
   - [Variable Neighborhood Search (VNS)](#46-variable-neighborhood-search-vns)
   - [Tabu Search (BT)](#47-tabu-search-tabu)
5. [Category 3: Metaheuristics & Evolutionary Algorithms](#5-category-3-metaheuristics--evolutionary-algorithms)
   - [Simulated Annealing (SA)](#51-simulated-annealing-sa)
   - [Genetic Algorithms (GA)](#52-genetic-algorithms-ga)
   - [Memetic Algorithms (MA)](#53-memetic-algorithms-ma)
   - [Scatter Search (Búsqueda Dispersa)](#54-scatter-search-scatter--bd)
6. [Category 4: Parallel Island Models](#6-category-4-parallel-island-models)
   - [Parallel Simulated Annealing (PSA)](#61-parallel-simulated-annealing-psa)
   - [Parallel Genetic Algorithm (PGA)](#62-parallel-genetic-algorithm-pga)
7. [Complexity Analysis & Empirical Benchmarks](#7-complexity-analysis--empirical-benchmarks)

---

## 1. Mathematical Formulation & Complexity

### The Problem
Given a complete undirected graph $G = (V, E)$ where:
- $V = \{v_1, v_2, \dots, v_n\}$ is a set of $n$ cities (vertices),
- $E = \{(v_i, v_j) : v_i, v_j \in V, i \neq j\}$ is the set of edges connecting all pairs of cities,
- $d(v_i, v_j) \ge 0$ is the standard TSPLIB `EUC_2D` rounded Euclidean distance metric between city $i$ and city $j$:
  $$d(v_i, v_j) = \text{round}\left(\sqrt{(x_i - x_j)^2 + (y_i - y_j)^2}\right)$$

The goal is to find a **Hamiltonian cycle** (a closed tour visiting every vertex exactly once and returning to the origin) that minimizes the total path cost:

$$\min_{\pi \in \Pi_n} C(\pi) = \sum_{i=1}^{n-1} d(v_{\pi(i)}, v_{\pi(i+1)}) + d(v_{\pi(n)}, v_{\pi(1)})$$

where $\Pi_n$ is the set of all valid permutations of $\{1, 2, \dots, n\}$.

### Combinatorial Explosion
The size of the search space for a symmetric TSP with $n$ cities is:

$$|\Pi_n| = \frac{(n - 1)!}{2}$$

| Cities ($n$) | Permutations $\frac{(n-1)!}{2}$ | Time at $10^9$ tours/sec |
| :--- | :--- | :--- |
| 10 | $181,440$ | $< 0.001$ seconds |
| 20 | $\approx 6.08 \times 10^{16}$ | $\approx 1.9$ years |
| 52 (Berlin52) | $\approx 7.77 \times 10^{65}$ | $\approx 2.4 \times 10^{49}$ years |
| 100 (KroA100) | $\approx 4.67 \times 10^{154}$ | Beyond age of universe |

Because TSP is **NP-hard**, exact enumeration is intractable for large $n$. Heuristic and metaheuristic algorithms find high-quality near-optimal solutions in milliseconds.

---

## 2. Taxonomy of Implemented Algorithms

```
                          ┌──────────────────────────────────────────────┐
                          │         TSP Optimization Algorithms          │
                          └──────────────────────┬───────────────────────┘
                                                 │
         ┌───────────────────────┬───────────────┴───────────────┬───────────────────────┐
         ▼                       ▼                               ▼                       ▼
┌──────────────────┐   ┌───────────────────┐           ┌───────────────────┐   ┌───────────────────┐
│   Constructive   │   │   Neighborhood    │           │  Metaheuristics   │   │  Parallel Island  │
│   Heuristics     │   │   & Local Search  │           │  & Evolutionary   │   │      Models       │
├──────────────────┤   ├───────────────────┤           ├───────────────────┤   ├───────────────────┤
│ • greedy         │   │ • rs (Random)     │           │ • sa (Annealing)  │   │ • psa (Par. SA)   │
│ • greedyls       │   │ • ls (2-Opt)      │           │ • ga (Genetic)    │   │ • pga (Par. GA)   │
│ • greedyls+      │   │ • vnd (VND)       │           │ • ma (Memetic)    │   └───────────────────┘
│ • grasp          │   │ • bmb (Multiboot) │           │ • scatter (BD)    │
│ • grasp+         │   │ • ils (Iterated)  │           └───────────────────┘
└──────────────────┘   │ • vns (Variable)  │
                       │ • tabu (Tabu)     │
                       └───────────────────┘
```

---

## 3. Category 1: Constructive Heuristics

Constructive heuristics build a tour incrementally from scratch by selecting the most promising city at each step according to a myopic rule.

### 3.1. Greedy Search (`greedy`)
* **Concept:** Starting from each city $v_{init} \in V$, the algorithm greedily connects to the closest unvisited city until a complete tour is formed. The best of all $n$ initializations is retained.
* **Pseudocode:**
  ```text
  for init from 0 to n-1:
      tour = [init]
      while |tour| < n:
          current = tour.last()
          next_city = argmin_{v not in tour} d(current, v)
          tour.append(next_city)
      best_tour = min(best_tour, tour)
  ```
* **Characteristics:** Extremely fast $O(n^2)$ deterministic heuristic. Susceptible to the "greedy trap" on closing edges.

### 3.2. Greedy + Local Search (`greedyls`)
* **Concept:** Executes deterministic Nearest Neighbor Greedy construction, followed by a full 2-Opt local search descent to untangle crossed edges.

### 3.3. Greedy + Extended Local Search (`greedyls+`)
* **Concept:** Starts with the deterministic greedy tour optimized with 2-opt BL, then applies 4 successive cycles of subpath perturbation ($s = n/4$) followed by 2-opt local search descent, retaining improvements.

### 3.4. GRASP (`grasp`)
* **Greedy Randomized Adaptive Search Procedure:**
  - **Constructive Phase:** Instead of strictly choosing the nearest city, all unvisited candidate neighbors are ranked by distance, and the top $l = \max(2, \lfloor 0.1 \cdot n \rfloor)$ cities form the **Restricted Candidate List (RCL)**. A city is randomly sampled from the RCL.
  - **Improvement Phase:** Runs 2-Opt local search on the completed tour.
  - **Repetitions:** 50 independent GRASP constructions + BLs.

### 3.5. GRASP Extended (`grasp+`)
* **Concept:** Executes 10 independent GRASP constructions. For each construction, applies 1 initial 2-opt BL followed by 4 mutation ($s = n/4$) + BL cycles ($10 \times 5 = 50$ BLs total).

---

## 4. Category 2: Neighborhood & Local Search

Local search methods start from a complete candidate solution and explore neighboring configurations generated by structural modifications.

### 4.1. Random Search (`rs`)
* **Concept:** Generates $M = 2000 \cdot n$ independent random permutations using uniform shuffling and records the overall best tour.

### 4.2. Local Search (2-Opt Best Improvement) (`ls`)
* **Concept:** Exhaustively evaluates all $\frac{n(n-1)}{2}$ pairwise node swaps or edge inversions in the neighborhood:
  $$\Delta C = (d(v_{i-1}, v_j) + d(v_i, v_{j+1})) - (d(v_{i-1}, v_i) + d(v_j, v_{j+1}))$$
  Moves to the strictly best neighbor in the full neighborhood until reaching a local optimum.

```
       Before (Crossed):                      After 2-Opt Inversion:
      A o─────────o B                            A o─────────o D
           \   /                                      |     |
            \ /                                       |     |
             X                                        |     |
            / \                                       |     |
           /   \                                      v     v
      C o─────────o D                            C o─────────o B
```

### 4.3. Variable Neighborhood Descent (`vnd`)
* **Concept:** Alternates systematically between 3 distinct neighborhood structures:
  1. $k=1$: Pairwise Node Swap ($N_1 = \frac{n(n-1)}{2}$).
  2. $k=2$: Subpath Shuffling of size 4 ($N_2 = 24(n-3)$).
  3. $k=3$: Subpath Shuffling of size 6 ($N_3 = 360(n-5)$).
  Resets $k \leftarrow 1$ whenever an improvement is found; increments $k \leftarrow k+1$ otherwise. Stops when $k > 3$ or $N_{total} \ge 2000 \cdot n$.

### 4.4. Basic Multiboot Search (`bmb`)
* **Concept:** Generates 50 independent random initial tours and applies full 2-Opt local search descent to each, keeping the best local optimum found.

### 4.5. Iterated Local Search (`ils`)
* **Concept:** Starts from a random initial tour optimized with BL. Then performs 49 iterations of:
  1. **Perturbation (Shaking):** Randomly shuffles a subpath of length $s = n/4$.
  2. **Local Search:** Descends to the local optimum via 2-opt.
  3. **Acceptance:** Replaces the incumbent tour if strictly superior.

### 4.6. Variable Neighborhood Search (`vns`)
* **Concept:** Similar to ILS, but systematically scales the shaking strength $k \in \{1, \dots, 5\}$ with subpath length $s = \lfloor n / (9-k) \rfloor$. When local search improves the incumbent tour, $k$ resets to 1; otherwise, $k$ steps to $k+1$ to escape deeper local basins.

### 4.7. Tabu Search (`tabu`)
* **Short-Term Memory (Tabu List):** Stores swapped city elements and their indices $(i, j, \text{pos}(i), \text{pos}(j))$, forbidding reversal for tenure $T_{size} = \lfloor n/2 \rfloor$.
* **Exploration:** Evaluates 40 candidate neighbors per iteration and selects the best non-tabu move.
* **Aspiration Criterion:** Tabu restrictions are bypassed if the candidate move strictly improves the global best solution found ($C(s) < C(s_{best})$).
* **Long-Term Memory (Edge Frequencies):** Maintains matrix $frec(i, j)$ counting consecutive edge occurrences in accepted tours.
* **Diversification Restarts (4 scheduled restarts at intervals of $10 \cdot n$ iterations):**
  - **25% probability:** Complete random shuffle.
  - **25% probability:** Re-intensification from global best solution.
  - **50% probability:** Long-Term Memory Greedy diversification on penalized distances:
    $$d'(i, j) = d(i, j) + 0.3 \cdot (d_{\max} - d_{\min}) \cdot \frac{frec(i, j)}{frec_{\max}}$$
  - **Tenure Scaling:** Dynamically varies $T_{size}$ by $\pm 50\%$ on each restart.

---

## 5. Category 3: Metaheuristics & Evolutionary Algorithms

### 5.1. Simulated Annealing (`sa`)
* **Metropolis Criterion:** A candidate move with $\Delta = C(s_{new}) - C(s_{current})$ is accepted with probability:
  $$P(\Delta, T) = \begin{cases} 1.0 & \text{if } \Delta \le 0 \\ \exp\left(-\frac{\Delta}{T}\right) & \text{if } \Delta > 0 \end{cases}$$
* **Cooling Schedule:** Cauchy cooling $T_{k+1} = \frac{T_k}{1 + \beta T_k}$ with $\beta = \frac{T_0 - T_f}{M \cdot T_0 \cdot T_f}$, $T_0 = \frac{\mu}{-\ln(\phi)} C(S_0)$ ($\mu=\phi=0.3 \implies 0.249175$), $T_f = 10^{-3}$, $L=20$, $M=100\cdot n$ stages ($2000\cdot n$ evals).
* **Neighborhood Operators:**
  - `swap`: Pairwise node exchange.
  - `invert`: Full 2-opt subsegment inversion between arbitrary indices $i$ and $j$.

### 5.2. Genetic Algorithms (`ga`)
* **Representation:** Permutation chromosome vector of size $n$.
* **Selection:** Binary tournament selection.
* **Crossover (OX - Order Crossover):**
  1. A swath between random cut points $[b_1, b_2]$ is copied directly from the father.
  2. The remaining slots are filled with unvisited cities in the circular order they appear in the mother.
* **Mutation:** Shuffled subpath of size $s = \lfloor n/8 \rfloor$ with probability $P_m = 0.1$.
* **Evolutionary Schemes:**
  - `Generational`: Population size $N=30$, crossover probability $P_c = 0.7$, offspring replace parents with elitist preservation of the best individual.
  - `Stationary`: Population size $N=30$, $P_c = 1.0$, 2 offspring replace the 2 worst individuals only if strictly superior.
* **Budget:** $2000 \cdot n$ evaluations.

### 5.3. Memetic Algorithms (`ma`)
* **Concept:** Synergistic hybridization of global genetic search with individual local search learning.
* **Hybridization Schedules (Population $N = 10$, $P_c = 0.7$, $P_m = 0.1$):**
  - `all,2`: Local search applied on the 2 best chromosomes every 1 generation.
  - `all,all`: Local search applied on all 10 chromosomes every 1 generation.
  - `10,2`: Local search applied on the 2 best chromosomes every 10 generations.
  - `10,all`: Local search applied on all 10 chromosomes every 10 generations.
* **Budget:** $2000 \cdot n$ total evaluations (counting all BL evaluations).

### 5.4. Scatter Search (`scatter` / `bd`)
* **Concept:** Population-based evolutionary metaheuristic maintaining a small Reference Set ($R$) of high quality and diverse solutions.
* **Components for TSP (P3 §3.3):**
  1. **Diverse Initial Population ($|P| = 30$):**
     - Maintains edge precedence frequency matrix $c_{ij}$ counting how often city $i$ precedes city $j$ in solutions in $P$.
     - Builds tours probabilistically: transition from $i$ to unvisited $j \in C_i$ follows:
       $$P_{ij} = \frac{1 - p_{ij}}{|C_i| - 1}, \quad p_{ij} = \frac{c_{ij}}{\sum_{k \in C_i} c_{ik}}$$
     - Optimizes each generated solution with 2-opt BL and updates $c_{ij}$.
  2. **Reference Set ($|R| = 6$):**
     - $R_1$ (Quality): Top $b_1 = 3$ solutions in $P$.
     - $R_2$ (Diversity): Top $b_2 = 3$ solutions in $P \setminus R_1$ maximizing average distance to $R_1$.
  3. **Circuit Distance Metric:**
     $$d(s_1, s_2) = \frac{1}{2} \sum_{i=1}^n \Big( f(s_1, s_2, i) + g(s_1, s_2, i) \Big)$$
     where $f=0$ if successor of $s_1[i]$ is identical in both solutions, $1$ otherwise; $g=0$ if predecessor is identical, $1$ otherwise.
  4. **Combination & Local Search:** Pairwise Order Crossover (OX) on all pairs in $R$, optimizing each offspring with 2-opt BL.
  5. **Reference Set Update:** Replaces members of $R$ when offspring improve quality or diversity.
  6. **Budget:** $2000 \cdot n$ total evaluations.

---

## 6. Category 4: Parallel Island Models

### 6.1. Parallel Simulated Annealing (`psa`)
* **Division Model:** $P = 5$ concurrent SA chains starting from independent random initial tours.
* **Continuous Cooling:** Each process performs $N = 20 \cdot n$ cooling steps with $L = 20$ neighbors per step ($2000 \cdot n$ total evals across processes).
* **Migration:** Every $M = n$ cooling steps (19 migrations total), all processes synchronize and adopt the global best tour found so far, continuing their continuous cooling schedule.

### 6.2. Parallel Genetic Algorithm (`pga`)
* **Island Model:** Divides the evolutionary population into $P = 4$ independent subpopulations of size $N = 10$.
* **Evolution:** Generational GA with elitism, running for $G = 50 \cdot n$ generations ($4 \times 10 \times 50n = 2000 \cdot n$ evals).
* **Topologies:**
  - `Ring`: Every $M = 2\cdot n$ generations, each island sends its best individual to replace the worst individual in Island $(i+1) \bmod P$.
  - `Star`: Island with the best mean fitness acts as master, exchanging champions with all peripheral islands.

```
       Ring Topology:                           Star Topology:

         Island 0                                  Island 1
          o────>o Island 1                            \
          ^     │                                      \
          │     │                              Island 2 ── Master ── Island 0
          │     v                                      /
         Island 3 <────o Island 2                     /
                                                   Island 3
```

---

## 7. Complexity Analysis & Empirical Benchmarks

### Theoretical Complexity Table

| Category | Algorithm | Identifier | Time Complexity (Per Iteration) | Space Complexity |
| :--- | :--- | :--- | :--- | :--- |
| **Constructive** | Greedy Search | `greedy` | $O(n^2)$ | $O(n^2)$ |
| | Greedy + Local Search | `greedyls` | $O(n^2) + O(I \cdot n^2)$ | $O(n^2)$ |
| | Extended Greedy | `greedyls+` | $O(k \cdot I \cdot n^2)$ | $O(n^2)$ |
| | GRASP | `grasp` | $O(k \cdot (n^2 + I \cdot n^2))$ | $O(n^2)$ |
| | GRASP Extended | `grasp+` | $O(k \cdot m \cdot I \cdot n^2)$ | $O(n^2)$ |
| **Neighborhood** | Random Search | `rs` | $O(M \cdot n)$ | $O(n^2)$ |
| | Local Search (2-Opt) | `ls` | $O(I \cdot n^2)$ | $O(n^2)$ |
| | Variable Neighborhood Descent | `vnd` | $O(I \cdot n^2)$ | $O(n^2)$ |
| | Basic Multiboot | `bmb` | $O(k \cdot I \cdot n^2)$ | $O(n^2)$ |
| | Iterated Local Search | `ils` | $O(k \cdot I \cdot n^2)$ | $O(n^2)$ |
| | Variable Neighborhood Search | `vns` | $O(k \cdot I \cdot n^2)$ | $O(n^2)$ |
| | Tabu Search | `tabu` | $O(N_{\max} \cdot (K + n))$ | $O(n^2)$ |
| **Metaheuristics** | Simulated Annealing | `sa` | $O(N_{\max})$ | $O(n^2)$ |
| | Genetic Algorithm | `ga` | $O(G \cdot P \cdot n)$ | $O(P \cdot n + n^2)$ |
| | Memetic Algorithm | `ma` | $O(G \cdot (P \cdot n + I \cdot n^2))$ | $O(P \cdot n + n^2)$ |
| | Scatter Search | `scatter` | $O(|P| \cdot I \cdot n^2 + |R|^2 \cdot I \cdot n^2)$ | $O(|P| \cdot n + n^2)$ |
| **Parallel** | Parallel SA | `psa` | $O(P \cdot N_{\max})$ | $O(P \cdot n^2)$ |
| | Parallel GA | `pga` | $O(P \cdot G \cdot n)$ | $O(P \cdot S \cdot n + n^2)$ |

*Notation:* $n$ = cities, $I$ = 2-opt descent steps, $k$ = iterations/restarts, $K$ = candidate neighbors (40), $P$ = population/processes, $G$ = generations, $S$ = island size, $|P|$ = diverse pool (30), $|R|$ = reference set (6).

### Empirical Benchmarks on TSPLIB Instances (Standardized 10 Seeds)

| Algorithm | Identifier | Berlin52 ($n=52$) Cost | KroA100 ($n=100$) Cost | A280 ($n=280$) Cost | Typical Exec Time |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Known Optimal Tour** | — | **7,542** | **21,282** | **2,579** | — |
| Greedy Search | `greedy` | **8,181** (+8.5%) | **24,698** (+16.1%) | **2,946** (+14.2%) | 4 ms |
| Greedy + Local Search | `greedyls` | **8,133** (+7.8%) | **22,585** (+6.1%) | **2,906** (+12.7%) | 6 ms |
| Greedy + Extended BL | `greedyls+` | **7,943** (+5.3%) | **22,585** (+6.1%) | **2,906** (+12.7%) | 57 ms |
| Simulated Annealing (Invert) | `sa_invert` | **8,012** (+6.2%) | **23,589** (+10.8%) | **2,880** (+11.7%) | 16 ms |
| GRASP | `grasp` | **8,456** (+12.1%) | **30,071** (+41.3%) | **5,030** (+95.0%) | 2.4 s |
| Iterated Local Search | `ils` | **8,795** (+16.6%) | **32,780** (+54.0%) | **5,544** (+115.0%) | 967 ms |
| Variable Neighborhood Search | `vns` | **9,261** (+22.8%) | **29,476** (+38.5%) | **4,976** (+92.9%) | 683 ms |
| Tabu Search (Edge LTM) | `tabu` | **9,090** (+20.5%) | **37,671** (+77.0%) | **5,224** (+102.6%) | 32 ms |
| Memetic Algorithm AM-(10, 10) | `ma_10_10` | **9,391** (+24.5%) | **38,089** (+79.0%) | **6,167** (+139.1%) | 707 ms |
| Parallel SA | `psa` | **9,492** (+25.9%) | **36,548** (+71.7%) | **6,256** (+142.6%) | 11 ms (2 ms scaled) |
| Scatter Search | `scatter` | **10,651** (+41.2%) | **42,206** (+98.3%) | **7,224** (+180.1%) | 68 ms |
| Local Search (2-Opt) | `ls` | **11,223** (+48.8%) | **39,751** (+86.8%) | **6,523** (+152.9%) | 76 ms |
| Parallel GA (Ring) | `pga_ring` | **9,425** (+25.0%) | **49,378** (+132.0%) | **10,994** (+326.3%) | 5.2 s (1.3 s scaled) |
| Random Search (Baseline) | `rs` | **22,754** (+201.7%) | **134,848** (+533.6%) | **29,479** (+1043.0%) | 239 ms |

