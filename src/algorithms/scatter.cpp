/**
 * @file scatter.cpp
 * @brief Implementation of Scatter Search (Búsqueda Dispersa) for TSP
 * @author Vikman Fernandez-Castro
 * @date Created: August 2026
 */

#include <algorithm>
#include <cfloat>
#include <limits>
#include <numeric>
#include <vector>

#include "../core/cycle.h"
#include "algorithms.h"

namespace Algorithms {

namespace {

constexpr int POP_SIZE = 30;   ///< Initial diverse population size |P|
constexpr int REF_B1 = 3;      ///< Number of quality solutions in reference set |R1|
constexpr int REF_B2 = 3;      ///< Number of diversity solutions in reference set |R2|
constexpr int REF_SIZE = REF_B1 + REF_B2; ///< Total reference set size |R| = 6

/**
 * @brief Calculate the circuit distance d(s1, s2) = 1/2 sum (f(s1, s2, i) + g(s1, s2, i))
 */
float circuitDistance(const Cycle &s1, const Cycle &s2) {
    const int n = s1.getSize();
    if (n < 3) return 0.0f;

    // Build position index lookup for s2
    std::vector<int> posInS2(n);
    for (int i = 0; i < n; ++i) {
        posInS2[s2.edgeAt(i)] = i;
    }

    int diffEdges = 0;
    for (int i = 0; i < n; ++i) {
        int u = s1.edgeAt(i);
        int next1 = s1.edgeAt((i + 1) % n);
        int prev1 = s1.edgeAt((i + n - 1) % n);

        int pos2 = posInS2[u];
        int next2 = s2.edgeAt((pos2 + 1) % n);
        int prev2 = s2.edgeAt((pos2 + n - 1) % n);

        if (next1 != next2) diffEdges++;
        if (prev1 != prev2) diffEdges++;
    }

    return 0.5f * static_cast<float>(diffEdges);
}

/**
 * @brief Order Crossover (OX) between two parent tours
 */
Cycle crossOX(const Cycle &father, const Cycle &mother, std::mt19937 &generator) {
    const int n = father.getSize();
    if (n < 3) return father;

    int bound1 = random(generator, n);
    int bound2 = random(generator, n);
    while (bound1 == bound2) {
        bound2 = random(generator, n);
    }
    if (bound1 > bound2) {
        std::swap(bound1, bound2);
    }

    Cycle son = father;
    std::vector<bool> inSwath(n, false);

    // 1. Copy swath [bound1, bound2] from father
    for (int i = bound1; i <= bound2; ++i) {
        int node = father.edgeAt(i);
        son.edgeAt(i) = node;
        inSwath[node] = true;
    }

    // 2. Fill remaining positions circularly from mother
    int sonIdx = (bound2 + 1) % n;
    int motherIdx = (bound2 + 1) % n;

    for (int count = 0; count < n; ++count) {
        int candidate = mother.edgeAt((motherIdx + count) % n);
        if (!inSwath[candidate]) {
            son.edgeAt(sonIdx) = candidate;
            sonIdx = (sonIdx + 1) % n;
            if (sonIdx == bound1) {
                sonIdx = (bound2 + 1) % n;
            }
        }
    }

    son.updateCost();
    return son;
}

} // anonymous namespace

void scatterSearch(Cycle &data, int count, unsigned int seed) {
    const int n = data.getSize();
    if (n < 3) return;

    const int nMax = count * n;
    int totalEvals = 0;
    std::mt19937 generator(seed);

    // Precedence frequency matrix c[i][j]: how many times city i precedes city j in solutions added to P
    std::vector<std::vector<int>> cFreq(n, std::vector<int>(n, 0));

    auto updatePrecedence = [&](const Cycle &tour) {
        for (int i = 0; i < n; ++i) {
            int u = tour.edgeAt(i);
            int v = tour.edgeAt((i + 1) % n);
            cFreq[u][v]++;
        }
    };

    // Phase 1: Diverse Initial Population Generation (|P| = 30)
    std::vector<Cycle> P;
    P.reserve(POP_SIZE);

    for (int p = 0; p < POP_SIZE && totalEvals < nMax; ++p) {
        Cycle tour = data;
        std::vector<bool> visited(n, false);

        int startCity = random(generator, n);
        tour.edgeAt(0) = startCity;
        visited[startCity] = true;

        for (int step = 1; step < n; ++step) {
            int prevCity = tour.edgeAt(step - 1);

            std::vector<int> candidates;
            candidates.reserve(n - step);
            for (int city = 0; city < n; ++city) {
                if (!visited[city]) {
                    candidates.push_back(city);
                }
            }

            int unvisitedCount = static_cast<int>(candidates.size());
            if (unvisitedCount == 1) {
                tour.edgeAt(step) = candidates[0];
                visited[candidates[0]] = true;
                break;
            }

            // Sum of frequencies for all unvisited candidates
            int sumFreq = 0;
            for (int cand : candidates) {
                sumFreq += cFreq[prevCity][cand];
            }

            int chosenCity = candidates[0];
            if (sumFreq == 0) {
                int idx = random(generator, unvisitedCount);
                chosenCity = candidates[idx];
            } else {
                std::vector<double> probs(unvisitedCount);
                for (int i = 0; i < unvisitedCount; ++i) {
                    double p_ij = static_cast<double>(cFreq[prevCity][candidates[i]]) / sumFreq;
                    probs[i] = (1.0 - p_ij) / static_cast<double>(unvisitedCount - 1);
                    if (probs[i] < 0.0) probs[i] = 0.0;
                }
                std::discrete_distribution<int> dist(probs.begin(), probs.end());
                chosenCity = candidates[dist(generator)];
            }

            tour.edgeAt(step) = chosenCity;
            visited[chosenCity] = true;
        }

        tour.updateCost();
        totalEvals += localSearch(tour);
        updatePrecedence(tour);
        P.push_back(tour);
    }

    if (P.empty()) {
        P.push_back(data);
    }

    // Phase 2: Construct Reference Set R (|R| = 6: R1 = 3 best quality, R2 = 3 diverse)
    std::sort(P.begin(), P.end(), [](const Cycle &a, const Cycle &b) {
        return a.getCost() < b.getCost();
    });

    std::vector<Cycle> R;
    R.reserve(REF_SIZE);

    int actualB1 = std::min(REF_B1, static_cast<int>(P.size()));
    for (int i = 0; i < actualB1; ++i) {
        R.push_back(P[i]);
    }

    // Select R2: solutions from P \ R1 maximizing average distance to R1
    std::vector<bool> inR(P.size(), false);
    for (int i = 0; i < actualB1; ++i) inR[i] = true;

    while (static_cast<int>(R.size()) < REF_SIZE && R.size() < P.size()) {
        float maxAvgDist = -1.0f;
        int bestCandidateIdx = -1;

        for (size_t i = actualB1; i < P.size(); ++i) {
            if (inR[i]) continue;

            float avgDist = 0.0f;
            for (int r = 0; r < actualB1; ++r) {
                avgDist += circuitDistance(P[i], R[r]);
            }
            avgDist /= static_cast<float>(actualB1);

            if (avgDist > maxAvgDist) {
                maxAvgDist = avgDist;
                bestCandidateIdx = static_cast<int>(i);
            }
        }

        if (bestCandidateIdx != -1) {
            inR[bestCandidateIdx] = true;
            R.push_back(P[bestCandidateIdx]);
        } else {
            break;
        }
    }

    Cycle bestOverall = R[0];

    // Phase 3: Reference Set Combination & Evolutionary Loop
    while (totalEvals < nMax) {
        std::vector<Cycle> newOffspring;
        newOffspring.reserve(R.size() * (R.size() - 1));

        // Generate offspring combinations on all pairs in R
        for (size_t i = 0; i < R.size(); ++i) {
            for (size_t j = i + 1; j < R.size(); ++j) {
                if (totalEvals >= nMax) break;

                Cycle son = crossOX(R[i], R[j], generator);
                totalEvals += localSearch(son);
                newOffspring.push_back(son);

                if (totalEvals >= nMax) break;

                Cycle daughter = crossOX(R[j], R[i], generator);
                totalEvals += localSearch(daughter);
                newOffspring.push_back(daughter);
            }
            if (totalEvals >= nMax) break;
        }

        if (newOffspring.empty()) break;

        // Check if any offspring entered R
        bool setChanged = false;

        for (const auto &child : newOffspring) {
            if (child.getCost() < bestOverall.getCost()) {
                bestOverall.setPath(child);
            }

            // Quality insertion in R1
            for (int r = 0; r < actualB1; ++r) {
                if (child.getCost() < R[r].getCost()) {
                    R[r].setPath(child);
                    setChanged = true;
                    break;
                }
            }

            // Diversity insertion in R2
            if (!setChanged && R.size() > static_cast<size_t>(actualB1)) {
                for (size_t r = actualB1; r < R.size(); ++r) {
                    if (child.getCost() < R[r].getCost()) {
                        R[r].setPath(child);
                        setChanged = true;
                        break;
                    }
                }
            }
        }

        // Diversification restart if reference set stagnated
        if (!setChanged) {
            for (size_t r = actualB1; r < R.size(); ++r) {
                R[r].shuffleSubpath(std::max(2, n / 4), generator);
                totalEvals += localSearch(R[r]);
                if (R[r].getCost() < bestOverall.getCost()) {
                    bestOverall.setPath(R[r]);
                }
            }
        }
    }

    data.setPath(bestOverall);
}

} // namespace Algorithms
