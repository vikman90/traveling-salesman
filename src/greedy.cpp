// 23 de Octubre de 2012

#include <cfloat>
#include "cycle.h"
#include "algorithms.h"

namespace Algorithms {

void greedy(Cycle &data)
{
    double dist;
    int last, nearestEdge = 0;
    Cycle curCycle = data;
    Cycle &bestCycle = data;

    for (int init = 0; init < data.getSize(); init++) {
        curCycle.sortPath();

        if (init != 0)
            curCycle.swap(0, init);

        for (int i = 1; i < data.getSize() - 1; i++) {
            last = curCycle.edgeAt(i - 1);
            dist = DBL_MAX;

            for (int j = i; j < data.getSize(); j++) {
                if (data.distance(last, curCycle.edgeAt(j)) < dist) {
                    nearestEdge = j;
                    dist = data.distance(last, curCycle.edgeAt(j));
                }
            }

            curCycle.swap(i, nearestEdge);
        }

        if (curCycle.getCost() < bestCycle.getCost()) {
            bestCycle.setPath(curCycle);
        }
    }
}

// Eficiencia O(n^2)
/*void greedy(Cycle &data)
{
    int last = 0, best = 0;
    double dist = DBL_MAX;
    double partialDist;

    data.sortPath();

    // Buscar la ciudad mas cercana al resto

    for (int i = 0; i < data.getSize(); i++) {
        partialDist = 0.0;

        for (int j = 0; j < data.getSize(); j++) {
            partialDist += data.distance(i, j);

            if (partialDist > dist)
                break;
        }

        if (partialDist < dist) {
            best = i;
            dist = partialDist;
        }
    }

    data.swap(0, best);

    // A partir del nodo seleccionado, rellenar.

    for (int k = 1; k < data.getSize(); k++) {
        last = data.edgeAt(k - 1);
        dist = DBL_MAX;

        for (int i = k; i < data.getSize(); i++) {
            if (data.distance(last, data.edgeAt(i)) < dist) {
                best = i;
                dist = data.distance(last, data.edgeAt(i));
            }
        }

        data.swap(k, best);
    }
}*/

/* Eficiencia O(n^3) por uso de exists() y recorrido superfluo
 void greedy(Cycle &data)
{
    int last;
    double dist = DBL_MAX;
    double partialDist;

    // Buscar la ciudad mas cercana al resto

    for (int i = 0; i < data.getSize(); i++) {
        partialDist = 0.0;

        for (int j = 0; j < data.getSize(); j++) {
            partialDist += data.distance(i, j);

            if (partialDist > dist)
                break;
        }

        if (partialDist < dist) {
            data.edgeAt(0) = i;
            dist = partialDist;
        }
    }

    // A partir del nodo seleccionado, rellenar.

    for (int k = 1; k < data.getSize(); k++) {
        last = data.edgeAt(k - 1);
        dist = DBL_MAX;

        for (int i = 0; i < data.getSize(); i++) {
            if (!data.exists(i, k)) {
                if (data.distance(last, i) < dist) {
                    data.edgeAt(k) = i;
                    dist = data.distance(last, i);
                }
            }
        }
    }
}*/

}
