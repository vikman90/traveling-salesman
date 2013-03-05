/**
 * @file cycle.h
 * @brief Declaración de la clase Cycle
 * @date 22 de Octubre de 2012
 */

#ifndef CYCLE_H
#define CYCLE_H

#include <random>

/**
 * @brief Clase Ciclo
 *
 * Proporciona la estructura de almacenamiento de un ciclo hamiltoniano de
 * puntos en 2D.
 */
class Cycle
{
public:

    /// Estructura de cada nodo del grafo
    struct Vertex
    {
        float x;
        float y;
    };

    /**
     * @brief Constructor por defecto
     *
     * Crea un grafo vacío.
     */
    Cycle();

    /**
     * @brief Constructor con tamaño asignado
     * @param size Tamaño del grafo.
     */
    explicit Cycle(int size);

    /**
     * @brief Constructor a partir de archivo
     * @param path Ruta de un archivo TSP.
     *
     * Si el archivo TSP no se puede leer, se fuerza el cierre del programa.
     * Para detectar errores, cree un grafo vacío y llame a loadTSP().
     */
    explicit Cycle(const char *path);

    /**
     * @brief Constructor de copias
     * @param other Grafo de origen.
     */
    Cycle(const Cycle &other);

    /// Destructor
    ~Cycle();

    /**
     * @brief Operador de asignación
     * @param other Objeto origen.
     * @return Referencia al propio objeto.
     */
    Cycle & operator=(const Cycle &other);

    /**
     * @brief Cargar nodos de un archivo TSP
     * @param path Ruta del archivo.
     * @post Se actualiza la matriz de distancias implícitamente.
     * @return Si se ha leído correctamente.
     */
    bool loadTsp(const char *path);

    /**
     * @brief Cargar ruta de un archivo TOUR
     * @param path Ruta del archivo.
     * @pre El tamaño de la ruta debe coincidir con el de los nodos cargados.
     * @return Si se ha leído correctamente.
     */
    bool loadTour(const char *path);

    /**
     * @brief Guardar ruta en un archivo TOUR
     * @param path Ruta del archivo destino.
     * @return Si se ha escrito correctamente.
     */
    bool saveTour(const char *path) const;

    /**
     * @brief Limpiar ruta
     * @post El coste se hace infinito
     */
    void clearPath();

    /**
     * @brief Establecer ruta
     * @param other Ciclo que contiene la ruta
     * @pre Los vértices de @p other deben coincidir con los del propio objeto.
     *
     * Copia la ruta del ciclo @p other, @b suponiendo que los nodos son los
     * mismos. Si el tamaño es distinto, el método devuelve @c false, pero si
     * algún vértice tiene posiciones diferentes, el comportamiento de getCost()
     * no está definido.
     *
     * En caso de que los vértices sean distintos, o quepa posibilidad de ello,
     * se debe usar el operador de asignación.
     *
     * @return Si ha tenido éxito la operación.
     */
    bool setPath(const Cycle &other);

    /**
     * @brief Ordenar ruta
     *
     * Ordena el vector de aristas trazando una ruta que recorre ordenadamente
     * todos los nodos almacenados.
     */
    void sortPath();

    /**
     * @brief Barajar ruta
     * @param generator Generador de números.
     *
     * Desordena el vector de aristas con el algoritmo Mersenne Twist. Este
     * algoritmo produce ciclos de periodo 2^19937 - 1 con una velocidad
     * similar a la función estándar de C.
     */
    void shufflePath(std::mt19937 &generator);

    /**
     * @brief Barajar ruta parcialmente
     * @param count Tamaño de la sublista.
     * @param generator Generador de números.
     *
     * Desordena la sublista de tamaño @p count a partir del índice @p begin.
     * Si la sublista es de longitud 2, se recomienda swap().
     */
    void shuffleSubpath(int count, std::mt19937 &generator);

    /**
     * @brief Invertir sublista
     * @param first Posicion de la primera arista
     * @param count Tamaño de la sublista.
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
     * @brief Coste total del ciclo (función objetivo)
     *
     * Este método es equivalente a cost(getSize()).
     *
     * @overload cost(int bound)
     * @return Sumatoria de distancias de la ruta completa.
     */
    inline float getCost() const
    {
        return cost;
    }

    /**
     * @brief Obtener tamaño
     * @return Número de nodos almacenados en el ciclo.
     */
    inline int getSize() const
    {
        return size;
    }

    /**
     * @brief Referencia constante a vértice
     * @param index Índice del vértice dentro del vector.
     * @pre @index debe estar en el rango [0, size[.
     * @return Referencia constante al vértice indicado.
     */
    inline const Vertex & vertexAt(int index) const
    {
        return vertices[index];
    }

    /**
     * @brief Referencia constante a arista (elemento de la solución)
     * @param index Índice de la arista dentro del vector.
     * @pre @index debe estar en el rango [0, size[.
     * @return Referencia constante a la arista indicada.
     */
    inline const int & edgeAt(int index) const
    {
        return edges[index];
    }

	/**
     * @brief Referencia a arista (elemento de la solución)
     * @param index Índice de la arista dentro del vector.
     * @pre @index debe estar en el rango [0, size[.
     * @return Referencia constante a la arista indicada.
     */
    inline int & edgeAt(int index)
    {
        return edges[index];
    }

    /**
     * @brief Distancia entre dos nodos
     * @param i Índice de uno de los vértices.
     * @param j Índice del otro vértice.
     * @pre @p i y @p j deben estar en el rango [0, size[.
     * @return Distancia euclídea entre los vértices indicados.
     */
    inline float distance(int i, int j) const
    {
        return distances[i * size + j];
    }

private:

    int size;           ///< Tamaño del grafo
    float cost;        ///< Coste del ciclo
    Vertex *vertices;   ///< Vector de nodos
    int *edges;         ///< Aristas como secuencia de nodos (vector solución)
    float *distances;  ///< Matriz simétrica de distancias

    /**
     * @brief Actualizar matriz de distancias
     *
     * Se debe llamar en caso de modificar alguno de los nodos, y es necesario
     * para que distance() y getCost() devuelvan resultados correctos.
     */
    void updateDistances();

public:
    /**
     * @brief Actualizar coste del ciclo
     *
     * Se debe llamar en caso de modificar alguno de los nodos o aristas. Es
     * necesario para que getCost() devuelva un resultado correcto.
     */
    void updateCost();
};

#endif /* CYCLE_H */
