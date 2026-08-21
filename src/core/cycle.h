/**
 * @file cycle.h
 * @brief Declaration of the Cycle class representing a TSP Hamiltonian cycle
 * @author Vikman Fernandez-Castro
 * @date Created: October 22, 2012
 * @date Modified: August 2026 (C++17 STL Modernization)
 */

#ifndef TSP_CORE_CYCLE_H
#define TSP_CORE_CYCLE_H

#include <cfloat>
#include <istream>
#include <random>
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Cycle Class
 *
 * Provides the storage and operations for a 2D Hamiltonian cycle on Euclidean graphs.
 * Fully modernized with STL containers (Rule of Zero) and C++ standard features.
 */
class Cycle {
public:
    /// 2D coordinate representation for a graph node
    struct Vertex {
        float x{0.0f};
        float y{0.0f};
    };

    /**
     * @brief Default constructor creating an empty graph.
     */
    Cycle();

    /**
     * @brief Constructor with allocated size
     * @param size Number of vertices in the graph.
     */
    explicit Cycle(int size);

    /**
     * @brief Constructor from a TSP file path
     * @param path Path to a TSPLIB formatted file.
     */
    explicit Cycle(const char *path);
    explicit Cycle(const std::string &path);

    // Rule of Zero: Compiler-generated special member functions
    Cycle(const Cycle &other) = default;
    Cycle(Cycle &&other) noexcept = default;
    ~Cycle() = default;
    Cycle &operator=(const Cycle &other) = default;
    Cycle &operator=(Cycle &&other) noexcept = default;

    /**
     * @brief Load nodes from a TSPLIB file
     * @param path File path.
     * @return true if successfully loaded, false otherwise.
     */
    bool loadTsp(const char *path);
    bool loadTsp(const std::string &path);

    /**
     * @brief Load tour path from a TSPLIB TOUR file
     * @param path File path.
     * @return true if successfully loaded, false otherwise.
     */
    bool loadTour(const char *path);
    bool loadTour(const std::string &path);

    /**
     * @brief Save tour route to a TSPLIB TOUR file
     * @param path Destination file path.
     * @return true if successfully saved, false otherwise.
     */
    bool saveTour(const char *path) const;
    bool saveTour(const std::string &path) const;

    /**
     * @brief Save graph nodes to a TSPLIB TSP file
     * @param path Destination file path.
     * @return true if successfully saved, false otherwise.
     */
    bool saveTsp(const char *path) const;
    bool saveTsp(const std::string &path) const;

    /**
     * @brief Load nodes from an input stream
     * @param stream Input stream containing TSPLIB content.
     * @return true if successfully read, false otherwise.
     */
    bool loadTspFromStream(std::istream &stream);

    /**
     * @brief Load tour path from an input stream
     * @param stream Input stream containing TOUR content.
     * @return true if successfully read, false otherwise.
     */
    bool loadTourFromStream(std::istream &stream);

    /**
     * @brief Load nodes from TSP string content
     * @param content String with TSPLIB TSP content.
     * @return true if successfully parsed, false otherwise.
     */
    bool loadTspFromString(const std::string &content);

    /**
     * @brief Load path from TOUR string content
     * @param content String with TSPLIB TOUR content.
     * @return true if successfully parsed, false otherwise.
     */
    bool loadTourFromString(const std::string &content);

    /**
     * @brief Export graph nodes to standard TSPLIB string format
     * @return Formatted TSP string.
     */
    [[nodiscard]] std::string getTspString() const;

    /**
     * @brief Export tour path to standard TSPLIB string format
     * @return Formatted TOUR string.
     */
    [[nodiscard]] std::string getTourString() const;

    /**
     * @brief Set graph vertices from a vector
     * @param newVertices Vector of 2D points.
     */
    void setVertices(const std::vector<Vertex> &newVertices);

    /**
     * @brief Set graph vertices from a raw buffer
     * @param newSize Number of vertices.
     * @param newVertices Pointer to vertex array.
     */
    void setVertices(int newSize, const Vertex *newVertices);

    /**
     * @brief Clear path and set cost to infinity
     */
    void clearPath();

    /**
     * @brief Copy tour path from another Cycle of matching size
     * @param other Source cycle.
     * @return true if size matched and path was copied, false otherwise.
     */
    bool setPath(const Cycle &other);

    /**
     * @brief Reset path to sequential canonical order (0, 1, ..., size - 1)
     */
    void sortPath();

    /**
     * @brief Randomly shuffle the entire tour using Mersenne Twister
     * @param generator Standard PRNG instance.
     */
    void shufflePath(std::mt19937 &generator);

    /**
     * @brief Randomly shuffle a contiguous subpath
     * @param count Size of the subpath to shuffle.
     * @param generator Standard PRNG instance.
     */
    void shuffleSubpath(int count, std::mt19937 &generator);

    /**
     * @brief Reverse a contiguous subpath (2-Opt neighborhood move)
     * @param first Starting index of the subpath.
     * @param count Length of the subpath to invert.
     */
    void invertSubpath(int first, int count);

    /**
     * @brief Swap two nodes in the tour sequence and update cost incrementally
     * @param i First node index in tour [0, size[.
     * @param j Second node index in tour [0, size[.
     */
    void swap(int i, int j);

    /**
     * @brief Get total tour cost (objective function value)
     */
    [[nodiscard]] inline float getCost() const noexcept {
        return cost;
    }

    /**
     * @brief Get number of nodes in the graph
     */
    [[nodiscard]] inline int getSize() const noexcept {
        return size;
    }

    /**
     * @brief Const reference to vertex at given index
     */
    [[nodiscard]] inline const Vertex &vertexAt(int index) const {
        return vertices[index];
    }

    /**
     * @brief Const reference to edge/tour node at index
     */
    [[nodiscard]] inline const int &edgeAt(int index) const {
        return edges[index];
    }

    /**
     * @brief Mutable reference to edge/tour node at index
     */
    [[nodiscard]] inline int &edgeAt(int index) {
        return edges[index];
    }

    /**
     * @brief Precalculated Euclidean distance between vertex i and vertex j
     */
    [[nodiscard]] inline float distance(int i, int j) const {
        return distances[i * size + j];
    }

    /**
     * @brief Access internal vertices vector
     */
    [[nodiscard]] inline const std::vector<Vertex> &getVertices() const noexcept {
        return vertices;
    }

    /**
     * @brief Access internal edges vector
     */
    [[nodiscard]] inline const std::vector<int> &getEdges() const noexcept {
        return edges;
    }

    /**
     * @brief Update precomputed distance matrix from current vertex coordinates
     */
    void updateDistances();

    /**
     * @brief Recalculate total tour cost from current edge sequence
     */
    void updateCost();

private:
    int size{0};
    float cost{FLT_MAX};
    std::vector<Vertex> vertices;
    std::vector<int> edges;
    std::vector<float> distances;
};

#endif // TSP_CORE_CYCLE_H
