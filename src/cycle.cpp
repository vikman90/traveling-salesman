/**
 * @file cycle.cpp
 * @brief Definición de la clase Cycle
 * @date 22 de Octubre de 2012
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <random>
#include "cycle.h"
#include "algorithms.h"

#define LINE_WIDTH 80   /// Longitud máxima para una línea

//------------------------------------------------------------------------------

/**
 * @brief Obtener el valor de una etiqueta en un archivo
 * @param file Flujo de datos.
 * @param result Cadena donde se almacenará el resultado.
 * @param label Etiqueta que se busca.
 *
 * Se busca una etiqueta (palabra que antecede a ':') y se escribe
 * en la cadena de resultado el resto de la línea. La búsqueda empieza
 * por el principio del archivo, y el puntero se queda al principio de
 * la línea siguiente.
 *
 * @return Si se ha encontrado la etiqueta.
 */
static bool fileTag(std::ifstream &file, char *result, const char *label)
{
    using namespace std;

    char _str[LINE_WIDTH];
    size_t length = strlen(label);

    file.seekg(0, ios_base::beg);

    do {
        file.getline(_str, LINE_WIDTH, ':');
        _str[length] = '\0';

        if (strcmp(_str, label))
            file.ignore(LINE_WIDTH, '\n');
        else {
            file.getline(result, LINE_WIDTH);
            return true;
        }
    } while (!file.fail());

    cerr << "Error de lectura: etiqueta <" << label << "> no encontrada.\n";
    return false;
}

//------------------------------------------------------------------------------

/**
 * @brief Verificar un atributo en un archivo
 * @param file Flujo de datos.
 * @param attrib Nombre del atributo (etiqueta).
 * @param value Valor que se espera que tenga el atributo.
 * @return Si se ha encontrado la etiqueta y el valor coincide.
 */
static bool fileAssert(std::ifstream &file, const char *attrib, const char *value)
{
    char _str[LINE_WIDTH];
    char *_ptr = _str;

    if (!fileTag(file, _str, attrib))
        return false;

    while (*_ptr == ' ')
        _ptr++;

    file.unget();

    if (strcmp(_ptr, value)) {
        std::cerr << "Error de lectura: Se esperaba <" << attrib << "=="
                  << value << "> y se obtuvo <" << _str << ">\n";
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

/**
 * @brief Buscar una línea dentro de un archivo
 * @param file Flujo de datos.
 * @param line Línea que se busca.
 * @post El puntero queda situado al principio de la línea siguiente.
 * @return Si se ha encontrado la línea.
 */
static bool fileFindLine(std::ifstream &file, const char *line)
{
    using namespace std;
    char _str[LINE_WIDTH];

    file.seekg(0, ios_base::beg);

    do {
        file.getline(_str, LINE_WIDTH);

        if (!strcmp(_str, line))
            return true;
    } while (!file.fail());

    cerr << "Error de lectura: texto <" << line << "> no encontrado.\n";
    return false;
}

//------------------------------------------------------------------------------
// Constructor por defecto

Cycle::Cycle()
{
    size = 0;
    cost = FLT_MAX;
    vertices = NULL;
    edges = NULL;
    distances = NULL;
}

//------------------------------------------------------------------------------
// Constructor con tamaño asignado

Cycle::Cycle(int size)
{
    this->size = size;
    this->cost = FLT_MAX;
    this->vertices = (Vertex*)calloc(sizeof(Vertex), size);
    this->edges = (int*)calloc(sizeof(int), size);
    this->distances = (float*)calloc(sizeof(float), size * size);
}

//------------------------------------------------------------------------------
// Constructor a partir de archivo

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
// Constructor de copias

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
// Operador de asignación

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

    // Ampliamos los vectores si es necesario (pero nunca encogemos)

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
// Cargar nodos de un archivo TSP

bool Cycle::loadTsp(const char *path)
{
    using namespace std;
    int newSize;
    char _str[LINE_WIDTH];
    ifstream file(path);

    if (!file) {
        cerr << "No se pudo abrir el archivo " << path << endl;
        return false;
    }

    // Cabecera

    if (!fileAssert(file, "TYPE", "TSP"))
        return false;

    if (!fileTag(file, _str, "DIMENSION"))
        return false;

    newSize = atoi(_str);

    if (newSize == 0) {
        cerr << "Error de lectura: dimension no reconocida.\n";
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

    // Leer vértices

    if (!fileFindLine(file, "NODE_COORD_SECTION"))
        return false;

    for (int i = 0; i < size; i++) {
        file.ignore(LINE_WIDTH, ' ');
        file >> vertices[i].x >> vertices[i].y;
        file.ignore(LINE_WIDTH, '\n');
    }

    cost = FLT_MAX;
    memset(edges, 0, sizeof(int) * size);
    file.close();
    updateDistances();

    return true;
}

//------------------------------------------------------------------------------
// Cargar ruta de un archivo TOUR

bool Cycle::loadTour(const char *path)
{
    using namespace std;
    int _size;
    char _str[LINE_WIDTH];
    ifstream file(path);

    if (!file) {
        cerr << "No se pudo abrir el archivo " << path << endl;
        return false;
    }

    // Cabecera

    if (!fileAssert(file, "TYPE", "TOUR"))
        return false;

    if (!fileTag(file, _str, "DIMENSION"))
        return false;

    _size = atoi(_str);

    if (_size != size) {
        cerr << "Error de lectura: la dimension del recorrido es incorrecta.\n";
        file.close();
        return false;
    }

    // Leer aristas

    if (!fileFindLine(file, "TOUR_SECTION"))
        return false;

    for (int i = 0; i < size; i++) {
        file >> edges[i];
        edges[i]--;
    }

    if (!file) {
        cerr << "Error de lectura: faltan nodos.\n";
        file.close();
        return false;
    }

    file.close();
    updateCost();
    return true;
}

//------------------------------------------------------------------------------
// Guardar ruta en un archivo TOUR

bool Cycle::saveTour(const char *path) const
{
    using namespace std;
    ofstream file(path);

    if (!file) {
        cerr << "No se pudo abrir el archivo " << path << endl;
        return false;
    }

    // Cabecera

    file << "NAME : " << path << endl;
    file << "TYPE : TOUR\n";
    file << "DIMENSION : " << size << endl;
    file << "TOUR_SECTION\n";

    // Lista de índices de nodos (aristas)

    for (int i = 0; i < size; i++)
        file << edges[i] + 1 << endl;

    file << "EOF";

    if (!file) {
        cerr << "Error de escritura en archivo.\n";
        file.close();
        return false;
    }

    file.close();
    return true;
}

//------------------------------------------------------------------------------
// Establecer ruta

bool Cycle::setPath(const Cycle &other)
{
    if (other.size != size)
        return false;

    memcpy(edges, other.edges, sizeof(int) * size);
    cost = other.cost;
    return true;
}

//------------------------------------------------------------------------------
// Limpiar ruta

void Cycle::clearPath()
{
    cost = FLT_MAX;
    memset(edges, 0, sizeof(int) * size);
}

//------------------------------------------------------------------------------
// Ordenar ruta

void Cycle::sortPath()
{
    for (int i = 0; i < size; i++)
        edges[i] = i;

    updateCost();
}

//------------------------------------------------------------------------------
// Barajar ruta

void Cycle::shufflePath(std::mt19937 &generator)
{
    const int bound = size - 1;
    int i, j;

    sortPath();

    /*
     * Se comienza intercambiando un nodo aleatorio por el último.
     * A partir de ahí recortamos el vector por la derecha y continuamos.
     */

    for (int k = 0; k < bound; k++) {
        i = bound - k;

        /*
         * Seleccionar el nodo a intercambiar por el del final.
         * Generar un valor aleatorio en [0, i]
         */

        j = Algorithms::random(generator, i + 1);

        register int auxEdge = edges[j];
        edges[j] = edges[i];
        edges[i] = auxEdge;
    }

    updateCost();
}

//------------------------------------------------------------------------------
// Barajar ruta parcialmente

void Cycle::shuffleSubpath(int count, std::mt19937 &generator)
{
    const int bound = count - 1;
    int begin;
    int i, j;

    /*
     * Seleccionar el índice de comienzo de la lista.
     * Generar un valor en [0, size[
     */

    begin = Algorithms::random(generator, size - count);

    /*
     * Se comienza intercambiando un nodo aleatorio por el último.
     * A partir de ahí recortamos el vector por la derecha y continuamos.
     */

    for (int k = 0; k < bound; k++) {
        i = bound - k;

        /*
         * Seleccionar el nodo a intercambiar por el del final.
         * Generar un valor aleatorio en [0, i]
         */

        j = Algorithms::random(generator, i + 1);

        register int auxEdge = edges[begin + j];
        edges[begin + j] = edges[begin + i];
        edges[begin + i] = auxEdge;
    }

    updateCost();
}

//------------------------------------------------------------------------------
// Invertir sublista

void Cycle::invertSubpath(int first, int count)
{
    const int last = first + count - 1;
    const int middle = count / 2;

    cost -= distance(edges[(first + size - 1) % size], edges[first]) + distance(edges[last], edges[last + 1]);

    for (int i = 0; i < middle; i++) {
        register int auxEdge = edges[first + i];
        edges[first + i] = edges[last - i];
        edges[last - i] = auxEdge;
    }

    cost += distance(edges[(first + size - 1) % size], edges[first]) + distance(edges[last], edges[last + 1]);
}

//------------------------------------------------------------------------------
// Intercambiar dos nodos en la ruta

void Cycle::swap(int i, int j)
{
    const int prevI = (i + size - 1) % size;
    const int nextI = (i + 1) % size;
    const int prevJ = (j + size - 1) % size;
    const int nextJ = (j + 1) % size;

    /*
     * Actualizar coste
     * Si los valores son adyacentes, solo hay que restar una vez la arista que
     * los une.
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

    // Intercambiar nodos

    register int auxEdge = edges[i];
    edges[i] = edges[j];
    edges[j] = auxEdge;
}

//------------------------------------------------------------------------------
// Actualizar matriz de distancias

void Cycle::updateDistances()
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j <= i; j++) {
            if (i == j)
                distances[i * size + j] = 0.0;
            else {
                register float x = vertices[i].x - vertices[j].x;
                register float y = vertices[i].y - vertices[j].y;
                distances[i * size + j] = distances[j * size + i] = floor(sqrt(x * x + y * y));
            }
        }
    }
}

//------------------------------------------------------------------------------
// Calcular coste del cidlo

void Cycle::updateCost()
{
    cost = 0.0;

    if (size < 2)
        return;

    for (int i = 1; i < size; i++)
        cost += distance(edges[i - 1], edges[i]);

    cost += distance(edges[0], edges[size - 1]);
}
