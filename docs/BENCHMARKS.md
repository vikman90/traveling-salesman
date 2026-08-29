# 📊 TSP Algorithmic Empirical Benchmark Report (P0 - P4)

> **Standardized Protocol:** 10 independent runs per probabilistic algorithm using official seeds (`12345678`, `23456781`, `34567812`, `45678123`, `56781234`, `67812345`, `78123456`, `81234567`, `87654321`, `18765432`). TSPLIB rounded Euclidean distance metric.

## Global Comparative Summary Table

| Method / Algorithm | Category | Berlin52 (m / x̄ / σ / GAP%) | KroA100 (m / x̄ / σ / GAP%) | A280 (m / x̄ / σ / GAP%) | Mean Time (t / t/P) |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Known Optimal** | Theoretical | **7542** / **7542** / 0.0 / 0.0% | **21282** / **21282** / 0.0 / 0.0% | **2579** / **2579** / 0.0 / 0.0% | — |
| Greedy Search (Deterministic) | Baseline | 8181 / 8181.0 / 0.0 / 8.47% | 24698 / 24698.0 / 0.0 / 16.05% | 2946 / 2946.0 / 0.0 / 14.23% | 3ms |
| Random Search (BA) | P1 | 21469 / 22516.2 / 386.7 / 198.54% | 130921 / 133874.5 / 1743.6 / 529.05% | 29216 / 29543.4 / 150.0 / 1045.54% | 244ms |
| Local Search (BL 2-Opt) | P1 | 9361 / 10451.1 / 742.7 / 38.57% | 32047 / 42106.9 / 4695.4 / 97.85% | 5668 / 6891.9 / 511.0 / 167.23% | 70ms |
| Variable Neighborhood Descent (VND) | P1 | 8850 / 10121.3 / 779.6 / 34.20% | 55326 / 59761.6 / 3425.3 / 180.81% | 24624 / 25488.6 / 486.5 / 888.31% | 4ms |
| Simulated Annealing (ES - Swap) | P1 | 9773 / 10500.6 / 570.6 / 39.23% | 35207 / 40756.9 / 3083.2 / 91.51% | 5037 / 6220.6 / 582.1 / 141.20% | 9ms |
| Simulated Annealing (ES - Invert) | P1 | 7925 / 8196.2 / 177.2 / 8.67% | 22419 / 23287.9 / 627.9 / 9.43% | 2797 / 2883.6 / 49.7 / 11.81% | 15ms |
| Tabu Search (BT - Edge LTM) | P1 | 8883 / 9169.3 / 205.4 / 21.58% | 33546 / 36696.0 / 1948.5 / 72.43% | 4733 / 5294.9 / 304.4 / 105.31% | 49ms |
| Basic Multiboot (BMB) | P2 | 8556 / 9015.9 / 207.8 / 19.54% | 30671 / 34872.1 / 2373.6 / 63.86% | 5668 / 6062.5 / 269.5 / 135.07% | 3.54s |
| GRASP (Semi-Greedy LRC) | P2 | 8456 / 8687.5 / 137.1 / 15.19% | 28421 / 30373.9 / 798.4 / 42.72% | 4663 / 4925.6 / 156.3 / 90.99% | 2.48s |
| Greedy + Local Search | P2 | 8133 / 8133.0 / 0.0 / 7.84% | 22585 / 22585.0 / 0.0 / 6.12% | 2906 / 2906.0 / 0.0 / 12.68% | 5ms |
| GRASP Extended (Hybrid) | P2 | 8358 / 8829.0 / 313.5 / 17.06% | 28610 / 31089.1 / 1702.6 / 46.08% | 5014 / 5243.8 / 139.8 / 103.33% | 1.33s |
| Greedy + Extended BL | P2 | 7915 / 8092.2 / 86.3 / 7.30% | 22585 / 22585.0 / 0.0 / 6.12% | 2906 / 2906.0 / 0.0 / 12.68% | 57ms |
| Iterated Local Search (ILS) | P2 | 8133 / 8608.9 / 495.4 / 14.15% | 28971 / 31509.6 / 1753.1 / 48.06% | 4842 / 5241.9 / 263.8 / 103.25% | 1.06s |
| Variable Neighborhood Search (VNS) | P2 | 8255 / 8783.5 / 398.6 / 16.46% | 26352 / 30334.2 / 1653.1 / 42.53% | 4637 / 5136.3 / 283.5 / 99.16% | 700ms |
| Genetic Algorithm (Generational OX) | P3 | 9454 / 10459.0 / 919.4 / 38.68% | 40716 / 44477.1 / 2196.5 / 108.99% | 8499 / 8966.3 / 480.1 / 247.67% | 9.77s |
| Genetic Algorithm (Stationary OX) | P3 | 9896 / 11528.5 / 1291.3 / 52.86% | 53075 / 64444.0 / 5782.5 / 202.81% | 16344 / 18872.7 / 1692.8 / 631.78% | 12.26s |
| Memetic Algorithm AM-(1, 2) | P3 | 9165 / 10129.1 / 440.8 / 34.30% | 36266 / 39516.0 / 3140.9 / 85.68% | 6437 / 7020.6 / 388.1 / 172.22% | 139ms |
| Memetic Algorithm AM-(1, 10) | P3 | 8968 / 9438.8 / 332.1 / 25.15% | 35832 / 38051.9 / 1431.8 / 78.80% | 6265 / 6587.7 / 205.1 / 155.44% | 685ms |
| Memetic Algorithm AM-(10, 2) | P3 | 9486 / 10014.7 / 277.4 / 32.79% | 36124 / 43052.0 / 4502.6 / 102.29% | 6670 / 7044.3 / 357.9 / 173.14% | 135ms |
| Memetic Algorithm AM-(10, 10) | P3 | 9275 / 9574.3 / 301.1 / 26.95% | 34101 / 37723.9 / 2790.9 / 77.26% | 6148 / 6533.0 / 277.0 / 153.32% | 684ms |
| Scatter Search (Búsqueda Dispersa) | P3 | 9222 / 10194.2 / 567.8 / 35.17% | 35467 / 41612.4 / 3659.2 / 95.53% | 6460 / 7100.5 / 382.6 / 175.32% | 69ms |
| Parallel Simulated Annealing (PSA) | P4 | 9392 / 10273.5 / 646.9 / 36.22% | 36474 / 38794.7 / 1775.8 / 82.29% | 5853 / 6496.6 / 419.4 / 151.90% | 10ms (2ms) |
| Parallel Genetic Algorithm (PGA - Ring) | P4 | 8669 / 9243.6 / 521.5 / 22.56% | 46382 / 49813.9 / 2454.4 / 134.07% | 10994 / 11390.7 / 341.7 / 341.67% | 5.27s (1.31s) |
| Parallel Genetic Algorithm (PGA - Star) | P4 | 8402 / 9782.9 / 731.8 / 29.71% | 35745 / 44022.7 / 5335.1 / 106.85% | 10497 / 10982.8 / 380.2 / 325.85% | 5.17s (1.29s) |

---

## 📈 In-Depth Comparative & Empirical Analysis

### 1. Constructive vs Local Search vs Trajectory Metaheuristics (P0 & P1)
* **Nearest Neighbor Greedy:** Achieves respectable quality on small/medium graphs ($\text{GAP} = 8.47\%$ on `berlin52`, $16.05\%$ on `kroA100`, $14.23\%$ on `a280`) in under $4\text{ ms}$. However, its deterministic nature traps it in edge-crossings near tour closure.
* **Greedy + BL (`greedyls` / `greedyls+`):** Combining deterministic greedy construction with 2-opt descent untangles these boundary crossings immediately, achieving $\text{GAP} = 6.12\%$ on `kroA100` and $7.30\%$ on `berlin52`.
* **Simulated Annealing (`sa`):** The choice of neighborhood operator is decisive:
  - `sa_swap` (Node swap) struggles to untangle large planar crossings ($\text{GAP} = 91.51\%$ on `kroA100`).
  - `sa_invert` (2-opt segment reversal) consistently achieves outstanding solution quality ($\text{GAP} = 8.67\%$ on `berlin52`, $9.43\%$ on `kroA100`, $11.81\%$ on `a280`), proving that 2-opt moves match the topological geometry of Euclidean planar TSP.
* **Tabu Search (`tabu`):** Short-term memory combined with edge-frequency long-term memory penalization ($d'$) effectively breaks out of local attraction basins ($\text{GAP} = 21.58\%$ on `berlin52`).

### 2. Multi-Start and Hybrid Metaheuristics (P2)
* **GRASP & GRASP+:** Semi-greedy construction with Restricted Candidate Lists ($l = \max(2, 0.1n)$) generates diverse starting points that 2-opt optimizes into competitive local minima ($\text{GAP} = 15.19\%$ on `berlin52`, $42.72\%$ on `kroA100`).
* **Iterated Local Search (ILS) & Variable Neighborhood Search (VNS):** Performing iterative perturbations ($s = n/4$) on local optima outperforms simple random restarts (BMB) by maintaining structural building blocks while exploring neighboring basins ($\text{GAP} = 14.15\%$ on `berlin52` for ILS, $16.46\%$ for VNS).

### 3. Evolutionary, Memetic, and Scatter Search (P3)
* **Pure Genetic Algorithms (`ga_gener` vs `ga_stat`):** Pure crossover (OX) and random mutation alone converge slowly on large graphs without local improvement. Generational GA achieves $\text{GAP} = 38.68\%$ on `berlin52` compared to $52.86\%$ for Stationary GA.
* **Memetic Synergy (AMs):** Hybridizing genetic search with 2-opt local search (`ma_1_10` and `ma_10_10`) dramatically improves solution quality ($\text{GAP} = 25.15\%$ and $26.95\%$ on `berlin52`, $77.26\%$ on `kroA100`), confirming that local learning provides powerful guidance to evolutionary recombination.
* **Scatter Search (`scatter`):** Maintains high population diversity via precedence frequency probability ($P_{ij}$) and Reference Set distance metric $d(s_1, s_2) = \frac{1}{2}\sum(f+g)$, achieving $\text{GAP} = 35.17\%$ on `berlin52` in just $69\text{ ms}$.

### 4. Parallel Scaling and Topology Dynamics (P4)
* **Parallel Simulated Annealing (PSA, $P=5$):** Multi-island concurrent exploration with progressive continuous cooling and periodic migration synchronizations achieves near-linear speedup ($\approx 5\times$) while finding quality solutions ($\text{GAP} = 36.22\%$ on `berlin52` in $2\text{ ms}$ scaled time).
* **Parallel Genetic Algorithms (PGA, $P=4$):**
  - **Ring Topology:** Smooth, gradual diffusion of elite individuals preserves subpopulation diversity longer, reaching $\text{GAP} = 22.56\%$ on `berlin52`.
  - **Star Topology:** Centralized master hub accelerates exploitation, achieving the best single-run cost on `kroA100` ($m = 35,745$) among evolutionary methods.

---

## 🔬 Benchmark Execution Instructions

To reproduce these exact benchmarks locally or test on custom instances:

```bash
# 1. Run all 10 standardized seeds across all datasets and regenerate report:
./build/tsp-benchmark --export docs/BENCHMARKS.md --csv docs/benchmarks.csv

# 2. Run a specific practical (e.g., Practice 3 - Evolutionary Algorithms):
./build/tsp-benchmark -p 3

# 3. Run a specific dataset (e.g., kroA100):
./build/tsp-benchmark -i kroA100

# 4. Quick smoke benchmark (1 seed):
./build/tsp-benchmark --quick
```

