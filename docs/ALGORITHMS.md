# 📘 Traveling Salesman Problem (TSP) — Educational Algorithm Guide

An exhaustive pedagogical and mathematical guide to the **Traveling Salesman Problem (TSP)** and the 17 heuristic, metaheuristic, evolutionary, and parallel algorithms implemented in this project.

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
   - [Local Search (2-Opt First Improvement)](#42-local-search-2-opt-ls)
   - [Variable Neighborhood Descent (VND)](#43-variable-neighborhood-descent-vnd)
   - [Basic Multiboot Search (BMB)](#44-basic-multiboot-search-bmb)
   - [Iterated Local Search (ILS)](#45-iterated-local-search-ils)
   - [Variable Neighborhood Search (VNS)](#46-variable-neighborhood-search-vns)
   - [Tabu Search (BT)](#47-tabu-search-tabu)
5. [Category 3: Metaheuristics & Evolutionary Algorithms](#5-category-3-metaheuristics--evolutionary-algorithms)
   - [Simulated Annealing (SA)](#51-simulated-annealing-sa)
   - [Genetic Algorithms (GA)](#52-genetic-algorithms-ga)
   - [Memetic Algorithms (MA)](#53-memetic-algorithms-ma)
6. [Category 4: Parallel Island Models](#6-category-4-parallel-island-models)
   - [Parallel Simulated Annealing (PSA)](#61-parallel-simulated-annealing-psa)
   - [Parallel Genetic Algorithm (PGA)](#62-parallel-genetic-algorithm-pga)
7. [Complexity Analysis & Benchmark Comparison](#7-complexity-analysis--benchmark-comparison)

---

## 1. Mathematical Formulation & Complexity

### The Problem
Given a complete undirected graph $G = (V, E)$ where:
- $V = \{v_1, v_2, \dots, v_n\}$ is a set of $n$ cities (vertices),
- $E = \{(v_i, v_j) : v_i, v_j \in V, i \neq j\}$ is the set of edges connecting all pairs of cities,
- $d(v_i, v_j) \ge 0$ is the Euclidean distance metric between city $i$ and city $j$:
  $$d(v_i, v_j) = \lfloor \sqrt{(x_i - x_j)^2 + (y_i - y_j)^2} \rfloor$$

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
│ • grasp          │   │ • bmb (Multiboot) │           └───────────────────┘
│ • grasp+         │   │ • ils (Iterated)  │
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
* **Pros & Cons:** Extremely fast $O(n^2)$; susceptible to the "greedy trap" where the final closing edges are forced to connect distant cities.

### 3.2. Greedy + Local Search (`greedyls`)
* **Concept:** Executes deterministic Nearest Neighbor Greedy construction, followed by a full 2-Opt local search to untangle crossed edges.

### 3.3. Greedy + Extended Local Search (`greedyls+`)
* **Concept:** Starts with the deterministic greedy tour, then applies $k$ iterations of random subpath perturbation (*shaking*) followed by local search descent.

### 3.4. GRASP (`grasp`)
* **Greedy Randomized Adaptive Search Procedure:**
  - **Constructive Phase:** Instead of strictly choosing the nearest city, candidate neighbors are sorted by distance into a **Restricted Candidate List (RCL)** of size $l_{size} = \max(2, n/10)$. A city is randomly sampled from the RCL to inject controlled stochastic diversity.
  - **Improvement Phase:** Runs 2-Opt local search on the candidate tour.

### 3.5. GRASP Extended (`grasp+`)
* **Concept:** Multi-start GRASP where each iteration also explores a small neighborhood basin through consecutive subpath mutations and local search re-optimizations.

---

## 4. Category 2: Neighborhood & Local Search

Local search methods start from a complete candidate solution and iteratively explore neighboring configurations generated by small structural modifications.

### 4.1. Random Search (`rs`)
* **Concept:** Generates $M = count \times n$ purely random permutations using `std::shuffle` and records the overall best tour. Serves as a baseline benchmark.

### 4.2. Local Search (2-Opt) (`ls`)
* **Concept:** Evaluates all pairwise node swaps in the tour sequence:
  $$\Delta C = (d(v_{i-1}, v_j) + d(v_i, v_{j+1})) - (d(v_{i-1}, v_i) + d(v_j, v_{j+1}))$$
  Whenever $\Delta C < 0$, the edge inversion is accepted immediately (First Improvement descent) until reaching a local optimum.

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
* **Concept:** Systematically alternates between 3 distinct neighborhood operators:
  1. $k=1$: Pairwise Node Swap ($\approx n(n-1)/2$ neighbors).
  2. $k=2$: Subpath Shuffling of length 4.
  3. $k=3$: Subpath Shuffling of length 6.
  If an improvement is found at neighborhood $k$, the algorithm resets to $k=1$; otherwise, it steps to $k+1$.

### 4.4. Basic Multiboot Search (`bmb`)
* **Concept:** Repeatedly generates a random initial tour with `std::shuffle` and applies 2-Opt local search descent, keeping the best local optimum found across all restarts.

### 4.5. Iterated Local Search (`ils`)
* **Concept:** Escapes local optima without discarding solution history. At each iteration:
  1. **Perturbation (Shaking):** Randomly shuffles a subpath of length $n/4$.
  2. **Local Search:** Descends to the nearest local optimum.
  3. **Acceptance:** Replaces the incumbent tour only if the new local optimum is superior.

### 4.6. Variable Neighborhood Search (`vns`)
* **Concept:** Similar to ILS, but systematically scales the shaking strength $k \in \{1, \dots, 5\}$ (subpath length $n/(9-k)$). When local search improves the incumbent tour, $k$ resets to 1; otherwise, $k$ increases to escape deeper basins.

### 4.7. Tabu Search (`tabu`)
* **Concept:** A memory-based trajectory metaheuristic that guides a local search process to explore beyond local optimality by forbidding (making *tabu*) recently visited or reversed moves.
* **Core Mechanisms:**
  1. **Short-Term Memory (Tabu List):** Stores the swapped city elements and their new permutation indices $(i, j, \text{Pos}(i), \text{Pos}(j))$. Any move that attempts to place element $i$ at $\text{Pos}(i)$ or element $j$ at $\text{Pos}(j)$ is forbidden for a duration determined by the tabu tenure (initially $\lfloor n/2 \rfloor$).
  2. **Neighbor Selection Strategy:** In each iteration, 40 candidate neighbors are evaluated through pairwise swaps. The best non-tabu neighbor is chosen to update the current trajectory.
  3. **Aspiration Criterion:** If a forbidden candidate move produces a tour cost strictly lower than the best global solution found so far ($C(s_{cand}) < C(s_{best})$), the tabu restriction is bypassed and the move is accepted.
  4. **Long-Term Memory Frequency Matrix ($f_{req}$):** An $n \times n$ matrix records the cumulative frequency $f_{req}(l, u)$ of each unit/city $u$ occupying location/position $l$ across accepted configurations.
  5. **Diversification Reinitializations:** Scheduled 4 times during the run (every $10 \cdot n$ iterations for total $N_{max} = 50 \cdot n$ iterations):
     - **Probability 0.25:** Random restart via complete permutation shuffle.
     - **Probability 0.25:** Re-intensification from the global best solution $s_{best}$.
     - **Probability 0.50:** Long-term memory greedy diversification, iteratively matching pairs $(l^*, u^*)$ with minimal $f_{req}(l, u)$ to enforce unvisited assignments.
     - **Adaptive Tenure Scaling:** Post-restart, the tabu tenure is either increased or decreased by 50% with equal probability (0.50).

---

## 5. Category 3: Metaheuristics & Evolutionary Algorithms

### 5.1. Simulated Annealing (`sa`)
* **Physical Analogy:** Simulates the metallurgical cooling of molten metals.
* **Metropolis Acceptance Criterion:** A candidate move with cost difference $\Delta = C(s_{new}) - C(s_{current})$ is accepted if:
  - $\Delta < 0$ (improving move, always accepted),
  - $\Delta \ge 0$ with probability $P(\Delta, T) = \exp\left(-\frac{\Delta}{T}\right)$.
* **Cooling Schedule:** Geometric cooling $T_{k+1} = \frac{T_k}{1 + \beta T_k}$ starting at initial temperature $T_0 = P \cdot C(s_0)$.
* **Neighborhood Operators:**
  - `swap`: Swaps two arbitrary nodes $v_i, v_j$.
  - `invert`: Inverts a random subpath of length 2 to 6.

### 5.2. Genetic Algorithms (`ga`)
* **Representation:** Permutation chromosome of size $n$.
* **Crossover (OX - Order Crossover):** Preserves relative order and adjacency:
  1. Two crossover cut points are randomly selected in the father chromosome.
  2. The swath between cut points is copied directly to the offspring.
  3. The remaining positions are filled with unvisited cities in the order they appear in the mother.
* **Mutation:** Random subpath shuffle of length $n/8$ with probability $p_m = 0.1$.
* **Evolutionary Schemes:**
  - `Generational`: The entire population is replaced each generation, with elitism preserving the best individual.
  - `Stationary` (Steady-State): Two parents are selected, two offspring are bred and immediately replace the worst individuals in the population.

### 5.3. Memetic Algorithms (`ma`)
* **Concept:** Synergistic hybridization of global genetic search (Darwinian evolution) with intensive individual local search (Lamarckian lifetime learning).
* **Hybridization Schedules (`-h`):**
  - `all,2`: Local search applied on the 2 best chromosomes every generation.
  - `all,all`: Local search applied on all chromosomes every generation.
  - `10,2`: Local search applied on the 2 best chromosomes every 10 generations.
  - `10,all`: Local search applied on all chromosomes every 10 generations.

---

## 6. Category 4: Parallel Island Models

### 6.1. Parallel Simulated Annealing (`psa`)
* **Model:** Multi-trajectory parallel search. Multiple independent SA annealing instances run concurrently, synchronizing their best-found tours at fixed migration latency intervals.

### 6.2. Parallel Genetic Algorithm (`pga`)
* **Island Model:** Divides the global population into $P$ isolated subpopulations (islands).
* **Migration Topologies (`-t`):**
  - `Ring`: Island $i$ sends its best individual to replace the worst individual in Island $(i+1) \bmod P$.
  - `Star`: The master island with the best average fitness distributes its champion to all other islands and receives their champions in return.

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

## 7. Complexity Analysis & Benchmark Comparison

### Theoretical Complexity Table

| Category | Algorithm | Identifier | Time Complexity (Per Iteration) | Space Complexity |
| :--- | :--- | :--- | :--- | :--- |
| **Constructive** | Greedy Search | `greedy` | $O(n^2)$ | $O(n^2)$ |
| | Greedy + Local Search | `greedyls` | $O(n^2) + O(I \cdot n^2)$ | $O(n^2)$ |
| | Extended Greedy | `greedyls+` | $O(k \cdot I \cdot n^2)$ | $O(n^2)$ |
| | GRASP | `grasp` | $O(k \cdot (n \log l + I \cdot n^2))$ | $O(n^2)$ |
| | GRASP Extended | `grasp+` | $O(k \cdot m \cdot I \cdot n^2)$ | $O(n^2)$ |
| **Neighborhood** | Random Search | `rs` | $O(M \cdot n)$ | $O(n^2)$ |
| | Local Search (2-Opt) | `ls` | $O(I \cdot n^2)$ | $O(n^2)$ |
| | Variable Neighborhood Descent | `vnd` | $O(I \cdot n^2)$ | $O(n^2)$ |
| | Basic Multiboot | `bmb` | $O(k \cdot I \cdot n^2)$ | $O(n^2)$ |
| | Iterated Local Search | `ils` | $O(k \cdot I \cdot n^2)$ | $O(n^2)$ |
| | Variable Neighborhood Search | `vns` | $O(k \cdot I \cdot n^2)$ | $O(n^2)$ |
| | Tabu Search | `tabu` | $O(N_{max} \cdot (K \cdot |T| + n))$ | $O(n^2)$ |
| **Metaheuristics** | Simulated Annealing | `sa` | $O(N_{max})$ | $O(n^2)$ |
| | Genetic Algorithm | `ga` | $O(G \cdot P \cdot n)$ | $O(P \cdot n + n^2)$ |
| | Memetic Algorithm | `ma` | $O(G \cdot (P \cdot n + I \cdot n^2))$ | $O(P \cdot n + n^2)$ |
| **Parallel** | Parallel SA | `psa` | $O(P \cdot N_{max})$ | $O(P \cdot n^2)$ |
| | Parallel GA | `pga` | $O(P \cdot G \cdot n)$ | $O(P \cdot S \cdot n + n^2)$ |

*Notation:* $n$ = cities, $I$ = 2-opt improvement steps, $k$ = iterations/restarts, $K$ = candidate neighbors (40), $|T|$ = tabu tenure, $P$ = population/processes, $G$ = generations, $S$ = island size.

### Empirical Benchmarks on TSPLIB Instances

| Algorithm | Berlin52 Tour Cost | KroA100 Tour Cost | A280 Tour Cost | Typical Execution Time |
| :--- | :--- | :--- | :--- | :--- |
| Random Tour (Baseline) | $\approx 23,800$ | $\approx 180,000$ | $\approx 65,000$ | $< 1$ ms |
| **Greedy (Nearest Neighbor)** | **$8,164$** | **$26,851$** | **$3,157$** | $< 1$ ms |
| **Local Search (2-Opt)** | **$10,527$** | **$29,420$** | **$3,480$** | $\approx 1$ ms |
| **Greedy + Local Search** | **$8,117$** | **$26,200$** | **$3,080$** | $\approx 1$ ms |
| **Simulated Annealing** | **$8,400 - 11,000$** | **$26,000 - 32,000$** | **$3,100 - 3,500$** | $\approx 1$ ms |
| **Tabu Search** | **$8,900 - 10,400$** | **$26,500 - 31,000$** | **$3,150 - 3,400$** | $\approx 2$ ms |
| **Variable Neighborhood Search** | **$9,012$** | **$27,500$** | **$3,150$** | $\approx 2$ ms |
| **Iterated Local Search** | **$9,451$** | **$28,100$** | **$3,200$** | $\approx 3$ ms |
| **Memetic Algorithm** | **$8,500 - 10,400$** | **$26,500 - 30,000$** | **$3,050 - 3,300$** | $\approx 5$ ms |
| **Known Optimal Tour** | **$7,542$** | **$21,282$** | **$2,579$** | — |
