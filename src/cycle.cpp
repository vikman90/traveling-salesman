/**
 * @file cycle.cpp
 * @brief Definition of the Cycle class
 * @date October 22, 2012
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <random>
#include "cycle.h"
#include "algorithms.h"

#define LINE_WIDTH 80   /// Maximum length for one line

static void trim(std::string &s)
{
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

//------------------------------------------------------------------------------

/**
 * @brief Get the value of a tag in a stream
 * @param file Data stream.
 * @param result String where the result will be stored.
 * @param label Label to be searched.
 * @return Whether the tag was found.
 */
static bool fileTag(std::istream &file, char *result, const char *label)
{
    file.clear();
    file.seekg(0, std::ios_base::beg);

    std::string line;
    std::string target(label);
    while (std::getline(file, line)) {
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string tag = line.substr(0, colon);
            trim(tag);
            if (tag == target) {
                std::string val = line.substr(colon + 1);
                trim(val);
                strncpy(result, val.c_str(), LINE_WIDTH - 1);
                result[LINE_WIDTH - 1] = '\0';
                return true;
            }
        }
    }

    std::cerr << "Reading error: label <" << label << "> not found.\n";
    return false;
}

//------------------------------------------------------------------------------

/**
 * @brief Check an attribute in a stream
 * @param file Data flow.
 * @param attrib Name of the attribute (tag).
 * @param value Value that the attribute is expected to have.
 * @return Whether the tag was found and the value matches.
 */
static bool fileAssert(std::istream &file, const char *attrib, const char *value)
{
    char result[LINE_WIDTH];
    if (!fileTag(file, result, attrib))
        return false;

    if (strcmp(result, value) != 0) {
        std::cerr << "Read error: Expected <" << attrib << "=="
                  << value << "> and got <" << result << ">\n";
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

/**
 * @brief Find a line within a stream
 * @param file Data flow.
 * @param line Line to search for.
 * @post The pointer is placed at the beginning of the next line.
 * @return whether the line was found.
 */
static bool fileFindLine(std::istream &file, const char *line)
{
    file.clear();
    file.seekg(0, std::ios_base::beg);

    std::string target(line);
    std::string current;
    while (std::getline(file, current)) {
        trim(current);
        if (current == target)
            return true;
    }

    std::cerr << "Error de lectura: texto <" << line << "> no encontrado.\n";
    return false;
}

//------------------------------------------------------------------------------
// Default constructor

Cycle::Cycle()
{
    size = 0;
    cost = FLT_MAX;
    vertices = NULL;
    edges = NULL;
    distances = NULL;
}

//------------------------------------------------------------------------------
// Constructor with assigned size

Cycle::Cycle(int size)
{
    this->size = size;
    this->cost = FLT_MAX;
    this->vertices = (Vertex*)calloc(sizeof(Vertex), size);
    this->edges = (int*)calloc(sizeof(int), size);
    this->distances = (float*)calloc(sizeof(float), size * size);
}

//------------------------------------------------------------------------------
// Constructor from file

Cycle::Cycle(const char *path)
{
    size = 0;
    cost = FLT_MAX;
    vertices = NULL;
    edges = NULL;
    distances = NULL;

    if (!loadTsp(path))
        exit(EXIT_FAILURE);
}

//------------------------------------------------------------------------------
// Copy constructor

Cycle::Cycle(const Cycle &other)
{
    size_t sVertices = sizeof(Vertex) * other.size;
    size_t sEdges = sizeof(int) * other.size;
    size_t sDistances = sizeof(float) * other.size * other.size;

    size = other.size;
    cost = other.cost;
    vertices = (Vertex*)malloc(sVertices);
    edges = (int*)malloc(sEdges);
    distances = (float*)malloc(sDistances);

    memcpy(vertices, other.vertices, sVertices);
    memcpy(edges, other.edges, sEdges);
    memcpy(distances, other.distances, sDistances);
}

//------------------------------------------------------------------------------
// Destructor

Cycle::~Cycle()
{
    free(vertices);
    free(edges);
    free(distances);
}

//------------------------------------------------------------------------------
// Assignment operator

Cycle & Cycle::operator=(const Cycle &other)
{
    size_t sVertices = sizeof(Vertex) * other.size;
    size_t sEdges = sizeof(int) * other.size;
    size_t sDistances = sizeof(float) * other.size * other.size;

    if (size == 0) {
        vertices = (Vertex*)malloc(sVertices);
        edges = (int*)malloc(sEdges);
        distances = (float*)malloc(sDistances);
    }

    // We expand the vectors if necessary (but never shrink)

    if (size < other.size) {
        vertices = (Vertex*)realloc(vertices, sVertices);
        edges = (int*)realloc(edges, sEdges);
        distances = (float*)realloc(distances, sDistances);
    }

    size = other.size;
    cost = other.cost;
    memcpy(vertices, other.vertices, sVertices);
    memcpy(edges, other.edges, sEdges);
    memcpy(distances, other.distances, sDistances);

    return *this;
}

//------------------------------------------------------------------------------
// Load nodes from a TSP file

bool Cycle::loadTsp(const char *path)
{
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Could not open file " << path << std::endl;
        return false;
    }
    return loadTspFromStream(file);
}

//------------------------------------------------------------------------------
// Load nodes from a stream

bool Cycle::loadTspFromStream(std::istream &file)
{
    using namespace std;
    int newSize;
    char _str[LINE_WIDTH];

    // Header

    if (!fileAssert(file, "TYPE", "TSP"))
        return false;

    if (!fileTag(file, _str, "DIMENSION"))
        return false;

    newSize = atoi(_str);

    if (newSize == 0) {
        cerr << "Read error: dimension not recognized.\n";
        return false;
    }

    if (newSize > size) {
        vertices = (Vertex*)realloc(vertices, sizeof(Vertex) * newSize);
        edges = (int*)realloc(edges, sizeof(int) * newSize);
        distances = (float*)realloc(distances, sizeof(float) * newSize * newSize);
    }

    size = newSize;

    if (!fileAssert(file, "EDGE_WEIGHT_TYPE", "EUC_2D"))
        return false;

    // Read vertices

    if (!fileFindLine(file, "NODE_COORD_SECTION"))
        return false;

    for (int i = 0; i < size; i++) {
        int id;
        if (!(file >> id >> vertices[i].x >> vertices[i].y)) {
            cerr << "Reading error: could not read vertex " << (i + 1) << "\n";
            return false;
        }
    }

    cost = FLT_MAX;
    memset(edges, 0, sizeof(int) * size);
    updateDistances();

    return true;
}

//------------------------------------------------------------------------------
// Load nodes from a string

bool Cycle::loadTspFromString(const std::string &content)
{
    std::istringstream stream(content);
    return loadTspFromStream(stream);
}

//------------------------------------------------------------------------------
// Load path from a TOUR file

bool Cycle::loadTour(const char *path)
{
    std::ifstream file(path);
    if (!file) {
        std::cerr << "No se pudo abrir el archivo " << path << std::endl;
        return false;
    }
    return loadTourFromStream(file);
}

//------------------------------------------------------------------------------
// Load path from a stream

bool Cycle::loadTourFromStream(std::istream &file)
{
    using namespace std;
    int _size;
    char _str[LINE_WIDTH];

    // Head

    if (!fileAssert(file, "TYPE", "TOUR"))
        return false;

    if (!fileTag(file, _str, "DIMENSION"))
        return false;

    _size = atoi(_str);

    if (_size != size) {
        cerr << "Reading error: the path dimension is incorrect.\n";
        return false;
    }

    // Read edges

    if (!fileFindLine(file, "TOUR_SECTION"))
        return false;

    for (int i = 0; i < size; i++) {
        if (!(file >> edges[i])) {
            cerr << "Error de lectura: faltan nodos.\n";
            return false;
        }
        edges[i]--;
    }

    updateCost();
    return true;
}

//------------------------------------------------------------------------------
// Load path from a string

bool Cycle::loadTourFromString(const std::string &content)
{
    std::istringstream stream(content);
    return loadTourFromStream(stream);
}

//------------------------------------------------------------------------------
// Save route to a TOUR file

bool Cycle::saveTour(const char *path) const
{
    std::ofstream file(path);
    if (!file) {
        std::cerr << "No se pudo abrir el archivo " << path << std::endl;
        return false;
    }
    file << getTourString();
    return file.good();
}

//------------------------------------------------------------------------------
// Save nodes to a TSP file

bool Cycle::saveTsp(const char *path) const
{
    std::ofstream file(path);
    if (!file) {
        std::cerr << "No se pudo abrir el archivo " << path << std::endl;
        return false;
    }
    file << getTspString();
    return file.good();
}

//------------------------------------------------------------------------------
// Export TSP as string

std::string Cycle::getTspString() const
{
    std::ostringstream out;
    out << "NAME : problem\n";
    out << "TYPE : TSP\n";
    out << "DIMENSION : " << size << "\n";
    out << "EDGE_WEIGHT_TYPE : EUC_2D\n";
    out << "NODE_COORD_SECTION\n";
    for (int i = 0; i < size; i++) {
        out << (i + 1) << " " << vertices[i].x << " " << vertices[i].y << "\n";
    }
    out << "EOF\n";
    return out.str();
}

//------------------------------------------------------------------------------
// Export TOUR as string

std::string Cycle::getTourString() const
{
    std::ostringstream out;
    out << "NAME : solution.tour\n";
    out << "TYPE : TOUR\n";
    out << "DIMENSION : " << size << "\n";
    out << "TOUR_SECTION\n";
    for (int i = 0; i < size; i++) {
        out << (edges[i] + 1) << "\n";
    }
    out << "EOF\n";
    return out.str();
}

//------------------------------------------------------------------------------
// Set vertices

void Cycle::setVertices(const std::vector<Vertex> &newVertices)
{
    setVertices(static_cast<int>(newVertices.size()), newVertices.data());
}

void Cycle::setVertices(int newSize, const Vertex *newVertices)
{
    if (newSize > size) {
        vertices = (Vertex*)realloc(vertices, sizeof(Vertex) * newSize);
        edges = (int*)realloc(edges, sizeof(int) * newSize);
        distances = (float*)realloc(distances, sizeof(float) * newSize * newSize);
    }

    size = newSize;

    if (newVertices && size > 0) {
        memcpy(vertices, newVertices, sizeof(Vertex) * size);
    }

    for (int i = 0; i < size; i++) {
        edges[i] = i;
    }

    updateDistances();
    updateCost();
}

//------------------------------------------------------------------------------
// Set path

bool Cycle::setPath(const Cycle &other)
{
    if (other.size != size)
        return false;

    memcpy(edges, other.edges, sizeof(int) * size);
    cost = other.cost;
    return true;
}

//------------------------------------------------------------------------------
// Clear path

void Cycle::clearPath()
{
    cost = FLT_MAX;
    memset(edges, 0, sizeof(int) * size);
}

//------------------------------------------------------------------------------
// Sort path

void Cycle::sortPath()
{
    for (int i = 0; i < size; i++)
        edges[i] = i;

    updateCost();
}

//------------------------------------------------------------------------------
// Shuffle path

void Cycle::shufflePath(std::mt19937 &generator)
{
    const int bound = size - 1;
    int i, j;

    sortPath();

    /*
     * We start by exchanging a random node for the last one.
     * From there, we cut the vector on the right and continue.
     */

    for (int k = 0; k < bound; k++) {
        i = bound - k;

        /*
         * Select the node to exchange for the one at the end.
         * Generate a random value in [0, i]
         */

        j = Algorithms::random(generator, i + 1);

        int auxEdge = edges[j];
        edges[j] = edges[i];
        edges[i] = auxEdge;
    }

    updateCost();
}

//------------------------------------------------------------------------------
// Shuffle subpath

void Cycle::shuffleSubpath(int count, std::mt19937 &generator)
{
    const int bound = count - 1;
    int begin;
    int i, j;

    /*
     * Select the starting index of the list.
     * Generate a value in [0, size[
     */

    begin = Algorithms::random(generator, size - count);

    /*
     * We start by exchanging a random node for the last one.
     * From there we cut the vector on the right and continue.
     */

    for (int k = 0; k < bound; k++) {
        i = bound - k;

        /*
         * Select the node to exchange for the one at the end.
         * Generate a random value in [0, i]
         */

        j = Algorithms::random(generator, i + 1);

        int auxEdge = edges[begin + j];
        edges[begin + j] = edges[begin + i];
        edges[begin + i] = auxEdge;
    }

    updateCost();
}

//------------------------------------------------------------------------------
// Reverse subpath

void Cycle::invertSubpath(int first, int count)
{
    const int last = first + count - 1;
    const int middle = count / 2;

    cost -= distance(edges[(first + size - 1) % size], edges[first]) + distance(edges[last], edges[last + 1]);

    for (int i = 0; i < middle; i++) {
        int auxEdge = edges[first + i];
        edges[first + i] = edges[last - i];
        edges[last - i] = auxEdge;
    }

    cost += distance(edges[(first + size - 1) % size], edges[first]) + distance(edges[last], edges[last + 1]);
}

//------------------------------------------------------------------------------
// Swap two nodes on the path

void Cycle::swap(int i, int j)
{
    const int prevI = (i + size - 1) % size;
    const int nextI = (i + 1) % size;
    const int prevJ = (j + size - 1) % size;
    const int nextJ = (j + 1) % size;

    /*
      * Update cost
      * If the values are adjacent, you only have to subtract the edge that
      * joins them once.
      */

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

    // Swap noded

    int auxEdge = edges[i];
    edges[i] = edges[j];
    edges[j] = auxEdge;
}

//------------------------------------------------------------------------------
// Update distance matrix

void Cycle::updateDistances()
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j <= i; j++) {
            if (i == j)
                distances[i * size + j] = 0.0;
            else {
                float x = vertices[i].x - vertices[j].x;
                float y = vertices[i].y - vertices[j].y;
                distances[i * size + j] = distances[j * size + i] = floor(sqrt(x * x + y * y));
            }
        }
    }
}

//------------------------------------------------------------------------------
// Calculate cost of the cycle

void Cycle::updateCost()
{
    cost = 0.0;

    if (size < 2)
        return;

    for (int i = 1; i < size; i++)
        cost += distance(edges[i - 1], edges[i]);

    cost += distance(edges[0], edges[size - 1]);
}
