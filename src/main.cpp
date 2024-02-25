// October 22, 2012

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

static enum Algorithm { None, Greedy, RS, LS, VND, SA,
                        GreedyLS, GreedyLSExt, BMB, Grasp, GraspExt, ILS, VNS,
						Genetic, Memetic, ParallelSA, ParallelGenetic
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
            cerr << "Nothing to do...\n";
            return EXIT_FAILURE;
        }
        break;

    case Greedy:
        Algorithms::greedy(data);
        break;

    case RS:
        Algorithms::randomSearch(data, count, seed);
        break;

    case LS:
        Algorithms::localSearch(data, seed);
        break;

    case VND:
        Algorithms::descendantSearch(data, count, seed);
        break;

    case SA:
        Algorithms::simulatedAnnealing(data, count, seed, generator);
        break;

    case GreedyLS:
        Algorithms::greedy_bl(data);
        break;

    case GreedyLSExt:
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

	case ParallelSA:
		Algorithms::parallelAnnealing(data, processes, count, migrLatency, seed);
		break;

	case ParallelGenetic:
		Algorithms::parallelGenetic(data, processes, size, count, migrLatency, topology, seed);
    }

    t1 = clock();

    if (pathTOUR_out != NULL)
        data.saveTour(pathTOUR_out);
    else {
        cout << "Path:\n";

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
            else if (!strcmp(argv[i], "rs")) {
                algorithm = RS;
                count = 2000;
                continue;
            } else if (!strcmp(argv[i], "ls")) {
                algorithm = LS;
                continue;
            } else if (!strcmp(argv[i], "vnd")) {
                algorithm = VND;
                count = 2000;
                continue;
            } else if (!strcmp(argv[i], "sa")) {
                algorithm = SA;
                count = 2000;
                continue;
            } else if (!strcmp(argv[i], "greedyls")) {
                algorithm = GreedyLS;
                continue;
            } else if (!strcmp(argv[i], "greedyls+")) {
                algorithm = GreedyLSExt;
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
			} else if (!strcmp(argv[i], "ga")) {
                algorithm = Genetic;
                count = 2000;
				size = 30;
                continue;
			} else if (!strcmp(argv[i], "ma")) {
                algorithm = Memetic;
                count = 2000;
				size = 10;
                continue;
            } else if (!strcmp(argv[i], "psa")) {
                algorithm = ParallelSA;
                count = 20;
				processes = 5;
                continue;
            } else if (!strcmp(argv[i], "pga")) {
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
            if (algorithm != SA)
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
    cout << "      Traveling Salesman Problem - Vikman Fernandez-Castro\n"
         << endl
         << "How to use:\n"
         << "  tsp [-a ALGORITHM [-n REP] [-s SEED] [-m METHOD]] [-c TOUR] [-o TOUR] TSP\n"
         << endl
         << "Load the TSP file and calculate the cost of a minimum route from a\n"
         << "TOUR file or based on an ALGORITHM implemented in the application.\n"
         << endl
         << "OPTIONS:\n"
         << "  -a    Apply an ALGORITHM to calculate the cost (overrides -c).\n"
         << "  -n    Perform the algorithm (REP*n) times or generations. Def: according to the script.\n"
         << "  -s    Set the SEED of the pseudorandom generator. Def: time().\n"
         << "  -m    Use the Neighbor Generation METHOD for Simulated Annealing.\n"
         << "  -c    Show the cost of the tour saved in the TOUR file.\n"
         << "  -o    Save the route to a TOUR file instead of displaying it.\n"
		 << "  -d    Set the width of the population. Def: according to the script.\n"
		 << "  -g    Use the evolutionary SCHEME for genetic algorithms.\n"
		 << "  -h    Select the HYBRIDIZATION type for memetic algorithms.\n"
		 << "  -p    Specify the number of parallel processes. Def: according to the script.\n"
		 << "  -l    Change the migration latency. Def: according to the script.\n"
		 << "  -t    Follow a TOPOLOGY in the parallel genetic algorithm.\n"
         << endl
         << "ALGORITMOS:\n"
         << "  greedy    Greedy search\n"
         << endl
         << "  rs        Random search\n"
         << "  ls        Local search\n"
         << "  vnd       Variable neighborhood descent search\n"
         << "  sa        Simulated annealing\n"
         << endl
         << "  greedyls  Greedy + Local search\n"
         << "  greedyls+ Greedy + Extended local search\n"
         << "  bmb       Basic multiboot search\n"
         << "  grasp     Greedy randomized adaptive search\n"
         << "  grasp+    Extended GRASP\n"
         << "  ils       Iterative local search\n"
         << "  vns       Variable neighborhood search\n"
		 << endl
		 << "  ga        Genetic algorithms\n"
		 << "  ma        Memetic algorithms\n"
		 << endl
		 << "  psa       Parallel simulated annealing\n"
		 << "  pga       Parallel genetic algorithms\n"
         << endl
         << "METHODS:\n"
         << "  swap     Swap [default]\n"
         << "  invert   Reverse subpath\n"
		 << endl
		 << "EVOLUTIONARY SCHEMES:\n"
		 << "  gener    Generational [default]\n"
		 << "  stat     Stationary\n"
		 << endl
		 << "TYPES OF HYBRIDATION:\n"
		 << "  all,2    All generations, on the 2 best chromosomes\n"
		 << "  all,all  All generations, over all chromosomes [default]\n"
		 << "  10,2     Every 10 generations, on the 2 best chromosomes\n"
		 << "  10,all   Every 10 generations, on all chromosomes\n"
		 << endl
		 << "TOPOLOGY MODELS:\n"
		 << "  star     Star\n"
		 << "  ring     Ring [default]\n";
}
