/**
 * @file cycle.cpp
 * @brief Implementation of the Cycle class
 */

#include "cycle.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>

namespace {

void trim(std::string &s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t')) {
        s.pop_back();
    }
    size_t start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t')) {
        start++;
    }
    if (start > 0) {
        s = s.substr(start);
    }
}

bool fileTag(std::istream &file, std::string &result, std::string_view label) {
    file.clear();
    file.seekg(0, std::ios_base::beg);

    std::string line;
    while (std::getline(file, line)) {
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string tag = line.substr(0, colon);
            trim(tag);
            if (tag == label) {
                result = line.substr(colon + 1);
                trim(result);
                return true;
            }
        }
    }
    return false;
}

bool fileAssert(std::istream &file, std::string_view attrib, std::string_view value) {
    std::string result;
    if (!fileTag(file, result, attrib)) {
        return false;
    }
    return (result == value);
}

bool fileFindLine(std::istream &file, std::string_view targetLine) {
    file.clear();
    file.seekg(0, std::ios_base::beg);

    std::string current;
    while (std::getline(file, current)) {
        trim(current);
        if (current == targetLine) {
            return true;
        }
    }
    return false;
}

} // anonymous namespace

//------------------------------------------------------------------------------
// Constructors

Cycle::Cycle()
    : size(0), cost(FLT_MAX) {}

Cycle::Cycle(int size)
    : size(size),
      cost(FLT_MAX),
      vertices(size),
      edges(size),
      distances(size * size, 0.0f) {
    std::iota(edges.begin(), edges.end(), 0);
}

Cycle::Cycle(const char *path)
    : Cycle() {
    if (!loadTsp(path)) {
        std::cerr << "Fatal error: could not load TSP file: " << path << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

Cycle::Cycle(const std::string &path)
    : Cycle(path.c_str()) {}

//------------------------------------------------------------------------------
// File I/O Methods

bool Cycle::loadTsp(const char *path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return false;
    }
    return loadTspFromStream(file);
}

bool Cycle::loadTsp(const std::string &path) {
    return loadTsp(path.c_str());
}

bool Cycle::loadTour(const char *path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return false;
    }
    return loadTourFromStream(file);
}

bool Cycle::loadTour(const std::string &path) {
    return loadTour(path.c_str());
}

bool Cycle::saveTour(const char *path) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open destination file: " << path << std::endl;
        return false;
    }
    file << getTourString();
    return file.good();
}

bool Cycle::saveTour(const std::string &path) const {
    return saveTour(path.c_str());
}

bool Cycle::saveTsp(const char *path) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open destination file: " << path << std::endl;
        return false;
    }
    file << getTspString();
    return file.good();
}

bool Cycle::saveTsp(const std::string &path) const {
    return saveTsp(path.c_str());
}

//------------------------------------------------------------------------------
// Stream and String Parsing

bool Cycle::loadTspFromStream(std::istream &stream) {
    if (!fileAssert(stream, "TYPE", "TSP")) {
        std::cerr << "Read error: TYPE is not TSP.\n";
        return false;
    }

    std::string dimStr;
    if (!fileTag(stream, dimStr, "DIMENSION")) {
        std::cerr << "Read error: DIMENSION tag not found.\n";
        return false;
    }

    int newSize = 0;
    try {
        newSize = std::stoi(dimStr);
    } catch (...) {
        std::cerr << "Read error: Invalid DIMENSION value: " << dimStr << "\n";
        return false;
    }

    if (newSize <= 0) {
        std::cerr << "Read error: dimension must be positive.\n";
        return false;
    }

    if (!fileAssert(stream, "EDGE_WEIGHT_TYPE", "EUC_2D")) {
        std::cerr << "Read error: EDGE_WEIGHT_TYPE is not EUC_2D.\n";
        return false;
    }

    if (!fileFindLine(stream, "NODE_COORD_SECTION")) {
        std::cerr << "Read error: NODE_COORD_SECTION not found.\n";
        return false;
    }

    size = newSize;
    vertices.resize(size);
    edges.resize(size);
    distances.resize(size * size, 0.0f);

    for (int i = 0; i < size; ++i) {
        int id = 0;
        if (!(stream >> id >> vertices[i].x >> vertices[i].y)) {
            std::cerr << "Read error: failed to read vertex at index " << (i + 1) << "\n";
            return false;
        }
    }

    std::iota(edges.begin(), edges.end(), 0);
    cost = FLT_MAX;
    updateDistances();
    return true;
}

bool Cycle::loadTourFromStream(std::istream &stream) {
    if (!fileAssert(stream, "TYPE", "TOUR")) {
        std::cerr << "Read error: TYPE is not TOUR.\n";
        return false;
    }

    std::string dimStr;
    if (!fileTag(stream, dimStr, "DIMENSION")) {
        std::cerr << "Read error: DIMENSION tag not found.\n";
        return false;
    }

    int tourDim = 0;
    try {
        tourDim = std::stoi(dimStr);
    } catch (...) {
        return false;
    }

    if (tourDim != size) {
        std::cerr << "Read error: TOUR dimension (" << tourDim << ") does not match graph size (" << size << ").\n";
        return false;
    }

    if (!fileFindLine(stream, "TOUR_SECTION")) {
        std::cerr << "Read error: TOUR_SECTION not found.\n";
        return false;
    }

    edges.resize(size);
    for (int i = 0; i < size; ++i) {
        if (!(stream >> edges[i])) {
            std::cerr << "Read error: Missing tour node at position " << i << "\n";
            return false;
        }
        edges[i]--; // 1-indexed to 0-indexed
    }

    updateCost();
    return true;
}

bool Cycle::loadTspFromString(const std::string &content) {
    std::istringstream stream(content);
    return loadTspFromStream(stream);
}

bool Cycle::loadTourFromString(const std::string &content) {
    std::istringstream stream(content);
    return loadTourFromStream(stream);
}

//------------------------------------------------------------------------------
// Serialization

std::string Cycle::getTspString() const {
    std::ostringstream out;
    out << "NAME : problem\n";
    out << "TYPE : TSP\n";
    out << "DIMENSION : " << size << "\n";
    out << "EDGE_WEIGHT_TYPE : EUC_2D\n";
    out << "NODE_COORD_SECTION\n";
    for (int i = 0; i < size; ++i) {
        out << (i + 1) << " " << vertices[i].x << " " << vertices[i].y << "\n";
    }
    out << "EOF\n";
    return out.str();
}

std::string Cycle::getTourString() const {
    std::ostringstream out;
    out << "NAME : solution.tour\n";
    out << "TYPE : TOUR\n";
    out << "DIMENSION : " << size << "\n";
    out << "TOUR_SECTION\n";
    for (int i = 0; i < size; ++i) {
        out << (edges[i] + 1) << "\n";
    }
    out << "EOF\n";
    return out.str();
}

//------------------------------------------------------------------------------
// Graph and Path Manipulations

void Cycle::setVertices(const std::vector<Vertex> &newVertices) {
    size = static_cast<int>(newVertices.size());
    vertices = newVertices;
    edges.resize(size);
    std::iota(edges.begin(), edges.end(), 0);
    distances.resize(size * size, 0.0f);

    updateDistances();
    updateCost();
}

void Cycle::setVertices(int newSize, const Vertex *newVertices) {
    size = newSize;
    if (newVertices && newSize > 0) {
        vertices.assign(newVertices, newVertices + newSize);
    } else {
        vertices.resize(newSize);
    }
    edges.resize(size);
    std::iota(edges.begin(), edges.end(), 0);
    distances.resize(size * size, 0.0f);

    updateDistances();
    updateCost();
}

void Cycle::clearPath() {
    cost = FLT_MAX;
    std::fill(edges.begin(), edges.end(), 0);
}

bool Cycle::setPath(const Cycle &other) {
    if (other.size != size) {
        return false;
    }
    edges = other.edges;
    cost = other.cost;
    return true;
}

void Cycle::sortPath() {
    std::iota(edges.begin(), edges.end(), 0);
    updateCost();
}

void Cycle::shufflePath(std::mt19937 &generator) {
    sortPath();
    std::shuffle(edges.begin(), edges.end(), generator);
    updateCost();
}

void Cycle::shuffleSubpath(int count, std::mt19937 &generator) {
    if (size <= 1 || count <= 1) {
        return;
    }
    count = std::min(count, size);
    std::uniform_int_distribution<int> startDist(0, size - count);
    int begin = startDist(generator);

    std::shuffle(edges.begin() + begin, edges.begin() + begin + count, generator);
    updateCost();
}

void Cycle::invertSubpath(int first, int count) {
    if (count <= 1 || size <= 1) {
        return;
    }
    const int last = first + count - 1;
    const int prevFirst = (first + size - 1) % size;
    const int nextLast = (last + 1) % size;

    cost -= distance(edges[prevFirst], edges[first]) + distance(edges[last], edges[nextLast]);
    std::reverse(edges.begin() + first, edges.begin() + first + count);
    cost += distance(edges[prevFirst], edges[first]) + distance(edges[last], edges[nextLast]);
}

void Cycle::swap(int i, int j) {
    if (i == j || size < 2) {
        return;
    }
    const int prevI = (i + size - 1) % size;
    const int nextI = (i + 1) % size;
    const int prevJ = (j + size - 1) % size;
    const int nextJ = (j + 1) % size;

    if (nextI == j) {
        cost -= distance(edges[prevI], edges[i]) + distance(edges[j], edges[nextJ]);
        cost += distance(edges[prevI], edges[j]) + distance(edges[i], edges[nextJ]);
    } else if (nextJ == i) {
        cost -= distance(edges[prevJ], edges[j]) + distance(edges[i], edges[nextI]);
        cost += distance(edges[prevJ], edges[i]) + distance(edges[j], edges[nextI]);
    } else {
        cost -= distance(edges[prevI], edges[i]) + distance(edges[i], edges[nextI]);
        cost -= distance(edges[prevJ], edges[j]) + distance(edges[j], edges[nextJ]);
        cost += distance(edges[prevI], edges[j]) + distance(edges[j], edges[nextI]);
        cost += distance(edges[prevJ], edges[i]) + distance(edges[i], edges[nextJ]);
    }

    std::swap(edges[i], edges[j]);
}

void Cycle::updateDistances() {
    for (int i = 0; i < size; ++i) {
        distances[i * size + i] = 0.0f;
        for (int j = 0; j < i; ++j) {
            float dx = vertices[i].x - vertices[j].x;
            float dy = vertices[i].y - vertices[j].y;
            float dist = std::floor(std::sqrt(dx * dx + dy * dy));
            distances[i * size + j] = dist;
            distances[j * size + i] = dist;
        }
    }
}

void Cycle::updateCost() {
    cost = 0.0f;
    if (size < 2) {
        return;
    }
    for (int i = 1; i < size; ++i) {
        cost += distance(edges[i - 1], edges[i]);
    }
    cost += distance(edges[0], edges[size - 1]);
}
