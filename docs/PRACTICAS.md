# 🎓 Universidad — Guía Completa de Prácticas de Metaheurísticas (P0 - P4)

Este documento recopila la especificación académica completa, parámetros normalizados, protocolos de evaluación experimental y correspondencia directa con la implementación en C++ para cada una de las 5 prácticas universitarias sobre el **Problema del Viajante de Comercio (TSP)**.

---

## 📑 Índice de Prácticas
1. [Práctica 0: Introducción al TSP y Métricas de TSPLIB](#práctica-0-introducción-al-tsp-y-métricas-de-tsplib)
2. [Práctica 1: Búsqueda por Trayectorias (BA, BL, VND, ES, BT)](#práctica-1-búsqueda-por-trayectorias-ba-bl-vnd-es-bt)
3. [Práctica 2: Algoritmos Multiarranque (BMB, GRASP, Greedy+BL, ILS, VNS)](#práctica-2-algoritmos-multiarranque-bmb-grasp-greedybl-ils-vns)
4. [Práctica 3: Algoritmos Genéticos, Meméticos y Búsqueda Dispersa (AG, AM, BD)](#práctica-3-algoritmos-genéticos-meméticos-y-búsqueda-dispersa-ag-am-bd)
5. [Práctica 4: Algoritmos Paralelos (PSA, PGA Anillo y Estrella)](#práctica-4-algoritmos-paralelos-psa-pga-anillo-y-estrella)
6. [Protocolo Estadístico y Tablas de Resultados](#protocolo-estadístico-y-tablas-de-resultados)

---

## Práctica 0: Introducción al TSP y Métricas de TSPLIB

### 1. Definición del Problema
Dado un conjunto de $n$ ciudades $V = \{v_1, \dots, v_n\}$ con coordenadas bidimensionales $(x_i, y_i)$, se busca la permutación $\pi \in \Pi_n$ que minimiza la longitud total del ciclo hamiltoniano:

$$\min_{\pi} C(\pi) = \sum_{i=1}^{n-1} d(v_{\pi(i)}, v_{\pi(i+1)}) + d(v_{\pi(n)}, v_{\pi(1)})$$

### 2. Métrica de Distancia TSPLIB (`EUC_2D`)
De acuerdo con el estándar internacional TSPLIB, las distancias entre nodos se redondean al entero más próximo:

$$d(i, j) = \text{round}\left(\sqrt{(x_i - x_j)^2 + (y_i - y_j)^2}\right)$$

### 3. Instancias de Prueba y Óptimos Conocidos
| Instancia | Ciudades ($n$) | Fichero TSP | Fichero Óptimo | Coste Óptimo Teórico |
| :--- | :---: | :--- | :--- | :---: |
| **Berlin52** | 52 | `data/berlin52.tsp` | `data/berlin52.opt.tour` | **7542** |
| **KroA100** | 100 | `data/kroA100.tsp` | `data/kroA100.opt.tour` | **21282** |
| **A280** | 280 | `data/a280.tsp` | `data/a280.opt.tour` | **2579** |

### 4. Algoritmo Heurístico Constructivo: Greedy (Vecino Más Próximo)
* **Comando CLI:** `./build/tsp -a greedy data/berlin52.tsp`
* **Complejidad:** $O(n^2)$. Determina $n$ recorridos comenzando desde cada ciudad y selecciona el mejor.

---

## Práctica 1: Búsqueda por Trayectorias (BA, BL, VND, ES, BT)

### 1. Búsqueda Aleatoria (BA / RS)
* **Parámetros:** $M = 2000 \cdot n$ evaluaciones aleatorias uniformes.
* **CLI:** `./build/tsp -a rs -n 2000 -s 12345678 data/berlin52.tsp`

### 2. Búsqueda Local (BL / LS 2-Opt)
* **Entorno:** 2-Opt exhaustivo (mejor mejora / best improvement) sobre todos los pares $(i, j)$.
* **CLI:** `./build/tsp -a ls -s 12345678 data/berlin52.tsp`

### 3. Búsqueda por Entornos Variables (VND)
* **Estructuras de Entorno:**
  1. $k=1$: Intercambio de dos nodos (Swap).
  2. $k=2$: Reordenación aleatoria de subsecuencia de longitud 4.
  3. $k=3$: Reordenación aleatoria de subsecuencia de longitud 6.
* **Criterio de Parada:** $k > 3$ o $N_{\max} = 2000 \cdot n$ evaluaciones.
* **CLI:** `./build/tsp -a vnd -n 2000 -s 12345678 data/berlin52.tsp`

### 4. Enfriamiento Simulado (ES / SA)
* **Esquema de Enfriamiento:** Cauchy modificado $T_{k+1} = \frac{T_k}{1 + \beta T_k}$ con $\beta = \frac{T_0 - T_f}{M \cdot T_0 \cdot T_f}$.
* **Parámetros:**
  - $T_0 = 0.249175 \cdot C(S_0)$ ($\mu = 0.3, \phi = 0.3$).
  - $T_f = 0.001$.
  - $L = 20$ vecinos por etapa de temperatura.
  - $M = 100 \cdot n$ etapas de enfriamiento ($2000 \cdot n$ evaluaciones totales).
* **Operadores de Vecindario:**
  - `swap`: Intercambio de dos nodos (`-m swap`).
  - `invert`: Inversión de subsegmento 2-opt (`-m invert`).
* **CLI:** `./build/tsp -a sa -n 2000 -m invert -s 12345678 data/berlin52.tsp`

### 5. Búsqueda Tabú (BT / Tabu)
* **Lista Tabú (Memoria a Corto Plazo):** Tamaño $T_{size} = \lfloor n/2 \rfloor$. Almacena $(i, j, pos(i), pos(j))$.
* **Exploración:** Evalúa 40 vecinos candidatos aleatorios por iteración.
* **Aspiración:** Si un movimiento tabú mejora la mejor solución global, se acepta.
* **Memoria a Largo Plazo (Frecuencia de Aristas):** Matriz $frec(i, j)$ de aristas consecutivas aceptadas.
* **Reinicializaciones (4 en total, cada $10\cdot n$ iteraciones):**
  - $25\%$: Aleatoria.
  - $25\%$: Mejor solución global hasta la fecha.
  - $50\%$: Greedy sobre distancias penalizadas $d'(i, j) = d(i, j) + 0.3 \cdot (d_{\max} - d_{\min}) \cdot \frac{frec(i, j)}{frec_{\max}}$.
  - Variación de tenencia tabú: $\pm 50\%$ uniforme.
* **CLI:** `./build/tsp -a tabu -n 50 -s 12345678 data/berlin52.tsp`

---

## Práctica 2: Algoritmos Multiarranque (BMB, GRASP, Greedy+BL, ILS, VNS)

### 1. Búsqueda Multiarranque Básica (BMB)
* **Algoritmo:** 50 arranques independientes generados uniformemente al azar, aplicando BL 2-Opt a cada uno.
* **CLI:** `./build/tsp -a bmb -n 50 -s 12345678 data/berlin52.tsp`

### 2. GRASP
* **Construcción:** Lista Restringida de Candidatos (LRC) con las $l = \max(2, \lfloor 0.1 \cdot n \rfloor)$ ciudades más próximas entre todas las no visitadas.
* **Mejora:** BL 2-Opt. 50 arranques independientes.
* **CLI:** `./build/tsp -a grasp -n 50 -s 12345678 data/berlin52.tsp`

### 3. GRASP Extendido (GRASP+)
* **Algoritmo:** 10 arranques de GRASP. En cada arranque, 1 BL inicial + 4 ciclos de mutación ($s = n/4$) y BL ($10 \times 5 = 50$ BLs en total).
* **CLI:** `./build/tsp -a grasp+ -n 10 -s 12345678 data/berlin52.tsp`

### 4. Greedy + BL Determinista
* **CLI:** `./build/tsp -a greedyls data/berlin52.tsp`

### 5. Greedy + BL Extendido (Greedy+BL+)
* **Algoritmo:** Toma el tour Greedy, aplica BL, y realiza 4 ciclos posteriores de mutación ($s = n/4$) y BL (5 BLs totales).
* **CLI:** `./build/tsp -a greedyls+ -n 5 -s 12345678 data/berlin52.tsp`

### 6. Búsqueda Local Reiterada (ILS)
* **Algoritmo:** Solución inicial aleatoria + BL. Realiza 49 ciclos de perturbación ($s = n/4$) + BL, aceptando solo si mejora la solución previa.
* **CLI:** `./build/tsp -a ils -n 50 -s 12345678 data/berlin52.tsp`

### 7. Búsqueda de Entorno Variable (VNS)
* **Estructuras de Agitación:** Longitud de sublista $s_k = \lfloor n / (9-k) \rfloor$ para $k \in \{1, \dots, 5\}$.
* **Descenso:** BL 2-Opt. Si mejora, $k \leftarrow 1$; en caso contrario, $k \leftarrow k+1$. Total: 50 ciclos.
* **CLI:** `./build/tsp -a vns -n 50 -s 12345678 data/berlin52.tsp`

---

## Práctica 3: Algoritmos Genéticos, Meméticos y Búsqueda Dispersa (AG, AM, BD)

### 1. Algoritmos Genéticos (AG / GA)
* **Cruce OX:** Segmento $[b_1, b_2]$ del padre conservado; ciudades restantes rellenadas en orden circular desde la madre.
* **Mutación:** Barajado de subsegmento $s = \lfloor n/8 \rfloor$ con probabilidad $P_m = 0.1$.
* **Variantes:**
  - **Generacional (`gener`):** Tamaño $N=30$, $P_c = 0.7$, reemplazo generacional con elitismo.
  - **Estacionario (`stat`):** Tamaño $N=30$, $P_c = 1.0$, 2 hijos compiten contra los 2 peores.
* **CLI:**
  - `./build/tsp -a ga -g gener -d 30 -n 2000 -s 12345678 data/berlin52.tsp`
  - `./build/tsp -a ga -g stat -d 30 -n 2000 -s 12345678 data/berlin52.tsp`

### 2. Algoritmos Meméticos (AM / MA)
* **Población:** $N = 10$, $P_c = 0.7$, $P_m = 0.1$. Presupuesto total: $2000 \cdot n$ evaluaciones (incluyendo BL).
* **Variantes de Hibridación:**
  1. `AM-(1, 2)` / `all,2`: Cada 1 generación, BL sobre los 2 mejores cromosomas.
  2. `AM-(1, 10)` / `all,all`: Cada 1 generación, BL sobre todos los 10 cromosomas.
  3. `AM-(10, 2)` / `10,2`: Cada 10 generaciones, BL sobre los 2 mejores cromosomas.
  4. `AM-(10, 10)` / `10,all`: Cada 10 generaciones, BL sobre todos los 10 cromosomas.
* **CLI:** `./build/tsp -a ma -h all,all -d 10 -n 2000 -s 12345678 data/berlin52.tsp`

### 3. Búsqueda Dispersa (BD / Scatter Search)
* **Población Inicial Diversa ($|P|=30$):** Matriz de precedencias $c_{ij}$, probabilidad $P_{ij} = \frac{1 - p_{ij}}{|C_i|-1}$ con $p_{ij} = \frac{c_{ij}}{\sum c_{ik}}$. BL en cada una.
* **Conjunto de Referencia ($|R|=6$):** $R_1 = 3$ mejores por calidad; $R_2 = 3$ que maximizan la distancia media al conjunto $R_1$.
* **Métrica de Distancia:** $d(s_1, s_2) = \frac{1}{2} \sum (f + g)$ (diferencia de aristas).
* **Combinación:** Cruce OX en todos los pares de $R$ + optimización con BL.
* **CLI:** `./build/tsp -a scatter -n 2000 -s 12345678 data/berlin52.tsp`

---

## Práctica 4: Algoritmos Paralelos (PSA, PGA Anillo y Estrella)

### 1. Enfriamiento Simulado Paralelo (PSA)
* **Modelo:** División de la búsqueda con $P = 5$ subalgoritmos concurrentes.
* **Enfriamiento Continuo:** $N = 20\cdot n$ etapas de enfriamiento por proceso ($L = 20$ vecinos/etapa), total $2000\cdot n$ evaluaciones globales.
* **Migración:** Cada $M = n$ etapas (19 migraciones), sincronización global con la mejor solución global encontrada.
* **CLI:** `./build/tsp -a psa -p 5 -n 20 -l 1 -s 12345678 data/berlin52.tsp`

### 2. Algoritmo Genético Distribuido (PGA)
* **Modelo:** Islas de subpoblaciones $P = 4$, subpoblación por isla $N = 10$.
* **Generaciones:** $G = 50 \cdot n$ generaciones ($4 \times 10 \times 50n = 2000\cdot n$ evaluaciones totales).
* **Topologías de Migración (cada $M = 2\cdot n$ generaciones):**
  - **Anillo (`ring`):** Cada isla envía su mejor individuo a la isla $(i+1) \bmod P$ (reemplaza al peor).
  - **Estrella (`star`):** La isla con mejor fitness medio actúa como maestra, intercambiando campeones con las islas esclavas.
* **CLI:**
  - `./build/tsp -a pga -t ring -p 4 -d 10 -n 50 -l 2 -s 12345678 data/berlin52.tsp`
  - `./build/tsp -a pga -t star -p 4 -d 10 -n 50 -l 2 -s 12345678 data/berlin52.tsp`

---

## Protocolo Estadístico y Tablas de Resultados

### 1. Las 10 Semillas Oficiales Normalizadas
Todos los algoritmos probabilísticos se ejecutan exactamente 10 veces usando las siguientes semillas pseudoaleatorias:
1. `12345678`
2. `23456781`
3. `34567812`
4. `45678123`
5. `56781234`
6. `67812345`
7. `78123456`
8. `81234567`
9. `87654321`
10. `18765432`

### 2. Estadísticos Requeridos
Por cada combinación de (Algoritmo, Instancia) se calculan:
* **Mejor Coste ($m$):** $\min_{i} C_i$
* **Media de Coste ($\bar{x}$):** $\frac{1}{10} \sum_{i=1}^{10} C_i$
* **Peor Coste ($p$):** $\max_{i} C_i$
* **Desviación Típica Muestral ($\sigma$):** $\sqrt{\frac{1}{9}\sum_{i=1}^{10}(C_i - \bar{x})^2}$
* **Tiempo Medio ($t$):** $\frac{1}{10}\sum_{i=1}^{10} t_i$
* **Tiempo Escalado ($t/P$):** Para algoritmos paralelos ($P=5$ en PSA, $P=4$ en PGA).
* **Desviación Porcentual respecto al Óptimo ($\text{GAP}\%$):** $\frac{\bar{x} - \text{Óptimo}}{\text{Óptimo}} \times 100\%$

### 3. Ejecución Automatizada con `tsp-benchmark`
La herramienta nativa `./build/tsp-benchmark` ejecuta automáticamente todo este protocolo:
```bash
# Ejecutar todas las 10 semillas sobre todas las prácticas e instancias y exportar a Markdown:
./build/tsp-benchmark --export docs/BENCHMARKS.md --csv benchmarks.csv

# Filtrar solo la práctica 3 sobre KroA100:
./build/tsp-benchmark -p 3 -i kroA100

# Comprobación rápida (1 semilla) para validar tests:
./build/tsp-benchmark --quick
```

