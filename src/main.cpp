// 22 de Octubre de 2012

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include "cycle.h"
#include "algorithms.h"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

static enum Algorithm { None, Greedy, BA, BL, VND, ES,
                        GreedyBL, GreedyBLExt, BMB, Grasp, GraspExt, ILS, VNS,
						Genetic, Memetic, ParallelES, ParallelGenetic
                      } algorithm = None;

static char *pathTSP = NULL;
static char *pathTOUR_in = NULL;
static char *pathTOUR_out = NULL;
static int count = 2000;
static int size = 30;
static int processes = 5;
static int migrLatency = 1;
static unsigned int seed = 0;
static Algorithms::NeighborGenerator generator = Algorithms::Swap;
static Algorithms::Scheme scheme = Algorithms::Generational;
static Algorithms::Hybridization hybridization = Algorithms::Everygen_Everychrom;
static Algorithms::Topology topology = Algorithms::Ring;

static bool config(int argc, char **argv);
static void printHelp();

static inline int round(double n)
{
	return (int)(n + 0.5);
}

int main(int argc, char **argv)
{
    clock_t t0, t1;
    Cycle data;
    seed = (unsigned int)time(NULL);

    if (!config(argc, argv)) {
        printHelp();
        return EXIT_FAILURE;
    }

    if (!data.loadTsp(pathTSP))
        return EXIT_FAILURE;

    t0 = clock();

    switch (algorithm) {
    case None:
        if (pathTOUR_in != NULL)
            data.loadTour(pathTOUR_in);
        else {
            cerr << "Nada que hacer...\n";
            return EXIT_FAILURE;
        }
        break;

    case Greedy:
        Algorithms::greedy(data);
        break;

    case BA:
        Algorithms::randomSearch(data, count, seed);
        break;

    case BL:
        Algorithms::localSearch(data, seed);
        break;

    case VND:
        Algorithms::descendantSearch(data, count, seed);
        break;

    case ES:
        Algorithms::simulatedAnnealing(data, count, seed, generator);
        break;

    case GreedyBL:
        Algorithms::greedy_bl(data);
        break;

    case GreedyBLExt:
        Algorithms::greedy_blext(data, count, seed);
        break;

    case BMB:
        Algorithms::basicMultibootSearch(data, count, seed);
        break;

    case Grasp:
        Algorithms::grasp(data, count, seed);
        break;

    case GraspExt:
        Algorithms::graspExt(data, count, seed);
        break;

    case ILS:
        Algorithms::iteratedLocalSearch(data, count, seed);
        break;

    case VNS:
        Algorithms::variableSearch(data, count, seed);
		break;

	case Genetic:
		Algorithms::genetic(data, size, count, scheme, seed);
		break;

	case Memetic:
		Algorithms::memetic(data, size, count, hybridization, seed);
		break;

	case ParallelES:
		Algorithms::parallelAnnealing(data, processes, count, migrLatency, seed);
		break;

	case ParallelGenetic:
		Algorithms::parallelGenetic(data, processes, size, count, migrLatency, topology, seed);
    }

    t1 = clock();

    if (pathTOUR_out != NULL)
        data.saveTour(pathTOUR_out);
    else {
        cout << "Recorrido:\n";

        for (int i = 0; i < data.getSize() - 1; i++)
            cout << data.edgeAt(i) + 1 << ", ";

        cout << data.edgeAt(data.getSize() - 1) + 1 << endl;
    }

	int seconds = round(double(t1 - t0) / CLOCKS_PER_SEC);
	int minutes = seconds / 60;
	seconds -= minutes * 60;

	cout << seed << "\t" << data.getCost() << "\t" << minutes << ":" << seconds << endl;

    return EXIT_SUCCESS;
}

static bool config(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-a")) {
            i++;

            if (!strcmp(argv[i], "greedy")) {
                algorithm = Greedy;
                continue;
            }
            else if (!strcmp(argv[i], "ba")) {
                algorithm = BA;
                count = 2000;
                continue;
            } else if (!strcmp(argv[i], "bl")) {
                algorithm = BL;
                continue;
            } else if (!strcmp(argv[i], "vnd")) {
                algorithm = VND;
                count = 2000;
                continue;
            } else if (!strcmp(argv[i], "es")) {
                algorithm = ES;
                count = 2000;
                continue;
            } else if (!strcmp(argv[i], "greedybl")) {
                algorithm = GreedyBL;
                continue;
            } else if (!strcmp(argv[i], "greedybl+")) {
                algorithm = GreedyBLExt;
                count = 5;
                continue;
            } else if (!strcmp(argv[i], "bmb")) {
                algorithm = BMB;
                count = 50;
                continue;
            } else if (!strcmp(argv[i], "grasp")) {
                algorithm = Grasp;
                count = 50;
                continue;
            } else if (!strcmp(argv[i], "grasp+")) {
                algorithm = GraspExt;
                count = 10;
                continue;
            } else if (!strcmp(argv[i], "ils")) {
                algorithm = ILS;
                count = 50;
                continue;
            } else if (!strcmp(argv[i], "vns")) {
                algorithm = VNS;
                count = 50;
                continue;
			} else if (!strcmp(argv[i], "ag")) {
                algorithm = Genetic;
                count = 2000;
				size = 30;
                continue;
			} else if (!strcmp(argv[i], "am")) {
                algorithm = Memetic;
                count = 2000;
				size = 10;
                continue;
            } else if (!strcmp(argv[i], "esparall")) {
                algorithm = ParallelES;
                count = 20;
				processes = 5;
                continue;
            } else if (!strcmp(argv[i], "agparall")) {
                algorithm = ParallelGenetic;
                count = 50;
				size = 10;
				migrLatency = 2;
				processes = 4;
                continue;
            }

            else
                return false;
        }
        else if (!strcmp(argv[i], "-n")) {
            if (algorithm == None)
                return false;

            count = strtol(argv[++i], NULL, 10);
            continue;
        }
        else if (!strcmp(argv[i], "-s")) {
            if (algorithm == None)
                return false;

            seed = strtoul(argv[++i], NULL, 10);
            continue;
        }
		else if (!strcmp(argv[i], "-d")) {
            if (algorithm == None)
                return false;

            size = strtol(argv[++i], NULL, 10);
            continue;
        }
		else if (!strcmp(argv[i], "-p")) {
            if (algorithm == None)
                return false;

            processes = strtol(argv[++i], NULL, 10);
            continue;
        }
		else if (!strcmp(argv[i], "-l")) {
            if (algorithm == None)
                return false;

            migrLatency = strtol(argv[++i], NULL, 10);
            continue;
        }
        else if (!strcmp(argv[i], "-m")) {
            if (algorithm != ES)
                return false;

            i++;

            if (!strcmp(argv[i], "swap")) {
                generator = Algorithms::Swap;
                continue;
            }
            else if (!strcmp(argv[i], "invert")) {
                generator = Algorithms::Invert;
                continue;
            }
            else
                return false;
        }
        else if (!strcmp(argv[i], "-c")) {
            if (i + 1 < argc) {
                pathTOUR_in = argv[++i];
                continue;
            }
            else
                return false;
        }
        else if (!strcmp(argv[i], "-o")) {
            if (i + 1 < argc) {
                pathTOUR_out = argv[++i];
                continue;
            }
            else
                return false;
        }
		else if (!strcmp(argv[i], "-g")) {
			if (algorithm != Genetic)
				return false;

			i++;

			if (!strcmp(argv[i], "gener")) {
				scheme = Algorithms::Generational;
				continue;
			} 
			else if (!strcmp(argv[i], "stat")) {
				scheme = Algorithms::Stationary;
				continue;
			}
			else
				return false;
		}
		else if (!strcmp(argv[i], "-h")) {
			if (algorithm != Memetic)
				return false;

			i++;

			if (!strcmp(argv[i], "all,2")) {
				hybridization = Algorithms::Everygen_Twochrom;
				continue;
			} 
			else if (!strcmp(argv[i], "all,all")) {
				hybridization = Algorithms::Everygen_Everychrom;
				continue;
			}
			else if (!strcmp(argv[i], "10,2")) {
				hybridization = Algorithms::Tengen_Twochrom;
				continue;
			}
			else if (!strcmp(argv[i], "10,all")) {
				hybridization = Algorithms::Tengen_Everychrom;
				continue;
			}
			else
				return false;
		}
		else if (!strcmp(argv[i], "-t")) {
			if (algorithm != ParallelGenetic)
				return false;

			i++;

			if (!strcmp(argv[i], "star")) {
				topology = Algorithms::Star;
				continue;
			} 
			else if (!strcmp(argv[i], "ring")) {
				topology = Algorithms::Ring;
				continue;
			} 
			else
				return false;
		}
        else if (i == argc - 1)
            pathTSP = argv[i];
        else
            return false;
    }

    return (pathTSP != NULL);
}

static void printHelp()
{
    cout << "      ALGORITMICA - Victor M. Fernandez Castro - UNIVERSIDAD DE GRANADA\n"
         << endl
         << "Modo de empleo:\n"
         << "  tsp [-a ALGORITMO [-n REP] [-s SEM] [-m METODO]] [-c TOUR] [-o TOUR] TSP\n"
         << endl
         << "Carga el archivo TSP y calcula el coste de un recorrido minimo a partir de un\n"
         << "fichero TOUR o bien basandose en un ALGORITMO implementado en la aplicacion.\n"
         << endl
         << "OPCIONES:\n"
         << "  -a    Aplica un ALGORITMO para calcular el coste (invalida a -c).\n"
         << "  -n    Realiza el algoritmo (REP*n) veces o generaciones. Def: segun el guion.\n"
         << "  -s    Establece la SEMilla del generador pseudoaleatorio. Def: time().\n"
         << "  -m    Emplea el METODO de generacion de vecinos para Enfriamiento Simulado.\n"
         << "  -c    Muestra el coste del recorrido guardado en el fichero TOUR.\n"
         << "  -o    Guarda el recorrido en un fichero TOUR en lugar de mostrarlo.\n"
		 << "  -d    Establece la amplitud de la poblacion. Def: segun el guion.\n"
		 << "  -g    Utiliza el ESQUEMA evolutivo para algoritmos geneticos.\n"
		 << "  -h    Selecciona el tipo de HIBRIDACION para algoritmos memeticos.\n"
		 << "  -p    Especifica el numero de procesos paralelos. Def: segun el guion.\n"
		 << "  -l    Cambia la latencia de migracion. Def: segun el guion.\n"
		 << "  -t    Sigue una TOPOLOGIA en el algoritmo genetico paralelo.\n"
         << endl
         << "ALGORITMOS:\n"
         << "  greedy    Busqueda voraz\n"
         << endl
         << "  ba        Busqueda aleatoria\n"
         << "  bl        Busqueda local\n"
         << "  vnd       Busqueda descendente de entorno variable\n"
         << "  es        Enfriamiento simulado\n"
         << endl
         << "  greedybl  Greedy + Busqueda local\n"
         << "  greedybl+ Greedy + Busqueda local extendida\n"
         << "  bmb       Busqueda multiarranque basica\n"
         << "  grasp     Busqueda avida, aleatoria y extendida\n"
         << "  grasp+    GRASP extendido\n"
         << "  ils       Busqueda local reiterada\n"
         << "  vns       Busqueda basada en entornos cambiantes\n"
		 << endl
		 << "  ag        Algoritmos geneticos\n"
		 << "  am        Algoritmos memeticos\n"
		 << endl
		 << "  esparall  Enfriamiento simulado paralelo\n"
		 << "  agparall  Algoritmos geneticos paralelos\n"
         << endl
         << "METODOS:\n"
         << "  swap     Intercambio [por defecto]\n"
         << "  invert   Inversion de sublista\n"
		 << endl
		 << "ESQUEMAS EVOLUTIVOS:\n"
		 << "  gener    Generacional [por defecto]\n"
		 << "  stat     Estacionario\n"
		 << endl
		 << "TIPOS DE HIBRIDACION:\n"
		 << "  all,2    Todas las generaciones, sobre los 2 mejores cromosomas\n"
		 << "  all,all  Todas las generaciones, sobre todos los cromosomas [por defecto]\n"
		 << "  10,2     Cada 10 generaciones, sobre los 2 mejores cromosomas\n"
		 << "  10,all   Cada 10 generaciones, sobre todos los cromosomas\n"
		 << endl
		 << "MODELOS DE TOPOLOGIA:\n"
		 << "  star     Estrella\n"
		 << "  ring     Anillo [por defecto]\n";
}
