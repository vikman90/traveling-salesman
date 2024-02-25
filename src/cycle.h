/**
 * @file cycle.h
 * @brief Declaration of the Cycle class
 * @date October 22, 2012
 */

#ifndef CYCLE_H
#define CYCLE_H

#include <random>

/**
 * @brief Cycle Class
 *
 * Provides the storage structure of a 2D point Hamiltonian cycle.
 */
class Cycle
{
public:

    /// Structure of each node of the graph
    struct Vertex
    {
        float x;
        float y;
    };

    /**
     * @brief Default constructor
     *
     * Creates an empty graph.
     */
    Cycle();

    /**
     * @brief Constructor with assigned size
     * @param size Size of the graph.
     */
    explicit Cycle(int size);

    /**
     * @brief Constructor from file
     * @param path Path of a TSP file.
     *
     * If the TSP file cannot be read, the program is forced to close.
     * To detect errors, create an empty graph and call loadTSP().
     */
    explicit Cycle(const char *path);

    /**
     * @brief Copy constructor
     * @param other Source graph.
     */
    Cycle(const Cycle &other);

    /// Destructor
    ~Cycle();

    /**
     * @brief Assignment operator
     * @param other Source object.
     * @return Reference to the object itself.
     */
    Cycle & operator=(const Cycle &other);

    /**
     * @brief Load nodes from a TSP file
     * @param path File path.
     * @post The distance matrix is updated implicitly.
     * @return Whether it was read correctly.
     */
    bool loadTsp(const char *path);

    /**
     * @brief Load path from a TOUR file
     * @param path File path.
     * @pre The size of the path must match the size of the loaded nodes.
     * @return Whether it was read correctly.
     */
    bool loadTour(const char *path);

    /**
     * @brief Save route to a TOUR file
     * @param path Path of the destination file.
     * @return Whether it was written correctly.
     */
    bool saveTour(const char *path) const;

    /**
     * @brief Clear path
     * @post The cost becomes infinite
     */
    void clearPath();

    /**
     * @brief Set path
     * @param other Loop containing the path
     * @pre The vertices of @p other must match those of the object itself.
     *
     * Copy the cycle path @p other, @b assuming the nodes are the same. If the
     * size is different, the method returns @c false, but if any vertex has
     * different positions, the behavior of getCost() is undefined.
     *
     * If the vertices are different, or there is a possibility of this, the
     * assignment operator must be used.
     *
     * @return Whether the operation was successful.
     */
    bool setPath(const Cycle &other);

    /**
     * @brief Sort path
     *
     * Sorts the vector of edges by tracing a route that runs through all the
     * stored nodes in an orderly manner.
     */
    void sortPath();

    /**
     * @brief Shuffle path
     * @param generator Number generator.
     *
     * Shuffles the edge vector with the Mersenne Twist algorithm. This
     * algorithm produces cycles of period 2^19937 - 1 with a speed similar to
     * the standard C function.
     */
    void shufflePath(std::mt19937 &generator);

    /**
     * @brief Shuffle subpath
     * @param count Size of the subpath.
     * @param generator Number generator.
     *
     * Shuffles the sublist of size @p count starting at index @p begin.
     * If the sublist is of length 2, swap() is recommended.
     */
    void shuffleSubpath(int count, std::mt19937 &generator);

    /**
     * @brief Reverse subpath
     * @param first Position of the first edge
     * @param count Size of the subpath.
     */
    void invertSubpath(int first, int count);

    /**
     * @brief Intercambiar dos nodos en la ruta (operador generar vecino)
     * @param i Índice de uno de los vértices.
     * @param j Índice del otro vértice.
     * @param @p i y @p j deben estar en el rango [0, size[.
     */
    void swap(int i, int j);

    /**
     * @brief Total cycle cost (objective function)
     *
     * This method is equivalent to cost(getSize()).
     *
     * @overload cost(int bound)
     * @return Sum of distances of the complete path.
     */
    inline float getCost() const
    {
        return cost;
    }

    /**
     * @brief Get size
     * @return Number of nodes stored in the cycle.
     */
    inline int getSize() const
    {
        return size;
    }

    /**
     * @brief Constant reference to vertex
     * @param index Index of the vertex within the vector.
     * @pre @index must be in the range [0, size[.
     * @return Constant reference to the indicated vertex.
     */
    inline const Vertex & vertexAt(int index) const
    {
        return vertices[index];
    }

    /**
     * @brief Constant reference to edge (solution element)
     * @param index Index of the edge within the vector.
     * @pre @index must be in the range [0, size[.
     * @return Constant reference to the indicated edge.
     */
    inline const int & edgeAt(int index) const
    {
        return edges[index];
    }

	/**
     * @brief Reference to edge (solution element)
     * @param index Index of the edge within the vector.
     * @pre @index must be in the range [0, size[.
     * @return Constant reference to the indicated edge.
     */
    inline int & edgeAt(int index)
    {
        return edges[index];
    }

    /**
     * @brief Distance between two nodes
     * @param i Index of one of the vertices.
     * @param j Index of the other vertex.
     * @pre @p i and @p j must be in the range [0, size[.
     * @return Euclidean distance between the indicated vertices.
     */
    inline float distance(int i, int j) const
    {
        return distances[i * size + j];
    }

private:

    int size;           ///< Graph size
    float cost;         ///< Cycle cost
    Vertex *vertices;   ///< Vector of nodes
    int *edges;         ///< Edges as sequence of nodes (solution vector)
    float *distances;   ///< Symmetric distance matrix

    /**
     * @brief Update distance matrix
     *
     * It must be called if any of the nodes are modified, and is necessary so
     * that distance() and getCost() return correct results.
     */
    void updateDistances();

public:
    /**
     * @brief Update cycle cost
     *
     * It must be called in case of modifying any of the nodes or edges. It is
     * required for getCost() to return a correct result.
     */
    void updateCost();
};

#endif /* CYCLE_H */
