# Implementación Paralela de Algoritmos de Búsqueda para el 15-Puzzle

## Información del Proyecto

**Curso:** Programación Paralela  
**Proyecto:** Parcial No. 2 - Paralelización de Solvers del 15-Puzzle  
**Fecha:** Octubre 2025  


---

## Resumen Ejecutivo

El presente proyecto constituye una implementación completa de algoritmos de búsqueda para la resolución del problema del 15-puzzle, incluyendo tanto versiones secuenciales como paralelas utilizando OpenMP. Se implementaron tres algoritmos de búsqueda: BFS (Breadth-First Search), A* con heurística h1 (fichas mal ubicadas), y A* con heurística h2 (distancia de Manhattan). Posteriormente, se desarrollaron versiones paralelas de estos algoritmos y se realizó un análisis exhaustivo de rendimiento, speedup y eficiencia.

Los resultados experimentales demuestran que:
- El algoritmo A* con heurística Manhattan (h2) es el más eficiente en versión secuencial, con un tiempo promedio de 12.79 ms.
- Las versiones paralelas logran speedups entre 2.09x y 2.40x con 4 threads.
- BFS paralelo alcanza la mayor eficiencia (60%) debido a su diseño con buffers locales que minimizan la contención.
- Los resultados se ajustan a las predicciones teóricas de la Ley de Amdahl con una precisión del 94.9%.

---

## Tabla de Contenidos

1. [Introducción](#1-introducción)
2. [Marco Teórico](#2-marco-teórico)
3. [Metodología](#3-metodología)
4. [Implementación](#4-implementación)
5. [Arquitectura del Sistema](#5-arquitectura-del-sistema)
6. [Resultados Experimentales](#6-resultados-experimentales)
7. [Análisis de Rendimiento](#7-análisis-de-rendimiento)
8. [Conclusiones](#8-conclusiones)
9. [Trabajo Futuro](#9-trabajo-futuro)
10. [Referencias](#10-referencias)
11. [Anexos](#11-anexos)

---

## 1. Introducción

### 1.1 Contexto del Problema

El 15-puzzle es un problema clásico de búsqueda en inteligencia artificial que consiste en un tablero de 4×4 con 15 fichas numeradas y un espacio vacío. El objetivo es alcanzar una configuración objetivo mediante movimientos válidos del espacio vacío. Este problema es NP-completo y presenta desafíos computacionales significativos, especialmente para configuraciones aleatorias.

### 1.2 Motivación

La paralelización de algoritmos de búsqueda es un área crítica en ciencias de la computación, con aplicaciones en robótica, planificación automática, sistemas de navegación y juegos. Este proyecto explora diferentes estrategias de paralelización y evalúa su impacto en el rendimiento.

### 1.3 Objetivos

#### Objetivo General
Implementar y analizar versiones secuenciales y paralelas de algoritmos de búsqueda para el 15-puzzle utilizando OpenMP.

#### Objetivos Específicos
1. Implementar tres algoritmos secuenciales: BFS, A*-h1 y A*-h2
2. Desarrollar versiones paralelas con OpenMP
3. Medir y analizar speedup, eficiencia y escalabilidad
4. Comparar estrategias de sincronización y su impacto
5. Validar resultados con la Ley de Amdahl
6. Extender la solución a tableros NxN genéricos

---

## 2. Marco Teórico

### 2.1 Algoritmos de Búsqueda

#### 2.1.1 Breadth-First Search (BFS)
Algoritmo de búsqueda no informada que explora el espacio de estados por niveles. Garantiza encontrar la solución óptima (menor número de movimientos) pero puede explorar un gran número de estados.

**Complejidad:**
- Temporal: O(b^d), donde b es el factor de ramificación y d la profundidad
- Espacial: O(b^d)

#### 2.1.2 A* (A-Star)
Algoritmo de búsqueda informada que utiliza una función de evaluación:

```
f(n) = g(n) + h(n)
```

Donde:
- g(n): costo real desde el inicio hasta el nodo n
- h(n): estimación heurística del costo desde n hasta el objetivo
- f(n): estimación del costo total del camino óptimo a través de n

**Propiedades:**
- Si h(n) es admisible (nunca sobreestima), A* encuentra la solución óptima
- La calidad de la heurística determina la eficiencia del algoritmo

### 2.2 Heurísticas Implementadas

#### 2.2.1 Heurística h1: Fichas Mal Ubicadas
Cuenta el número de fichas que no están en su posición objetivo.

```cpp
int h1(board) = count(tile != target_position)
```

**Características:**
- Admisible: nunca sobreestima el costo real
- Poco informada: no considera la distancia real
- Overhead computacional bajo

#### 2.2.2 Heurística h2: Distancia de Manhattan
Suma las distancias Manhattan de cada ficha a su posición objetivo.

```cpp
int h2(board) = Σ |current_row - target_row| + |current_col - target_col|
```

**Características:**
- Admisible y más informada que h1
- Proporciona mejores estimaciones
- Mayor overhead computacional pero compensa con menos expansiones

### 2.3 Paralelización con OpenMP

OpenMP (Open Multi-Processing) es una API para programación paralela de memoria compartida que utiliza directivas de compilador para paralelizar código secuencial.

**Directivas Principales Utilizadas:**
- `#pragma omp parallel`: Crea región paralela
- `#pragma omp for`: Distribuye iteraciones de bucle entre threads
- `#pragma omp critical`: Sección crítica (acceso exclusivo)
- `#pragma omp atomic`: Operación atómica

### 2.4 Métricas de Rendimiento

#### 2.4.1 Speedup
```
S(p) = T₁ / Tₚ
```
Donde T₁ es el tiempo con 1 procesador y Tₚ con p procesadores.

#### 2.4.2 Eficiencia
```
E(p) = S(p) / p = T₁ / (p × Tₚ)
```
Indica qué tan bien se utilizan los recursos.

#### 2.4.3 Ley de Amdahl
```
Sₘₐₓ = 1 / (s + (1-s)/p)
```
Donde s es la fracción secuencial del programa.

---

## 3. Metodología

### 3.1 Diseño Experimental

#### 3.1.1 Entorno de Pruebas
- **Sistema Operativo:** Windows 10/11 y Linux (Docker)
- **Compilador:** g++ 11.4.0 con banderas `-std=c++11 -O2 -fopenmp`
- **Hardware:** CPU con 4-8 núcleos físicos
- **Lenguaje:** C++11

#### 3.1.2 Casos de Prueba
Se utilizaron 10 configuraciones diferentes del 15-puzzle con complejidades variables:
- Puzzles simples: 2-5 movimientos de solución
- Puzzles intermedios: 6-10 movimientos
- Puzzles complejos: 11-15 movimientos

#### 3.1.3 Procedimiento de Medición
1. Compilación con optimización -O2
2. Ejecución de 10 repeticiones por algoritmo
3. Cálculo de tiempo promedio (excluyendo min/max)
4. Medición con `std::chrono::high_resolution_clock`
5. Variación de número de threads: 1, 2, 4, 8

### 3.2 Representación de Datos

#### 3.2.1 Estado del Tablero
Representado como un string de 16 caracteres:
```cpp
string board = "ABCDEFGHIJKLMNO#";
```
Ventajas:
- Comparación O(1) con hash
- Copia eficiente
- Uso directo en estructuras STL

#### 3.2.2 Conversión de Coordenadas
```cpp
// Lineal a 2D
row = position / 4
col = position % 4

// 2D a lineal
position = row * 4 + col
```

---

## 4. Implementación

### 4.1 Módulos del Sistema

#### 4.1.1 `board_moves.cpp`
**Propósito:** Generación de movimientos válidos

**Funciones principales:**
```cpp
vector<string> generateMoves(const string& board);
```

**Algoritmo:**
1. Localizar posición del espacio vacío
2. Calcular coordenadas (fila, columna)
3. Probar 4 direcciones: arriba, abajo, izquierda, derecha
4. Validar límites del tablero
5. Generar nuevos estados válidos

**Complejidad:** O(1) por llamada, genera 2-4 vecinos

#### 4.1.2 `board_printer.cpp`
**Propósito:** Visualización del tablero

**Funciones principales:**
```cpp
void printBoard(const string& board);
```

#### 4.1.3 `board_available.cpp`
**Propósito:** Validación de solvabilidad

**Funciones principales:**
```cpp
bool isSolvable(const string& board);
```

**Criterio:** Conteo de inversiones debe ser par

### 4.2 Algoritmos Secuenciales

#### 4.2.1 BFS Secuencial (`bsp_puzzle_solver.cpp`)

**Estructuras de datos:**
```cpp
queue<pair<string, int>> q;           // Cola FIFO
unordered_set<string> visited;        // Estados visitados
```

**Pseudocódigo:**
```
Función solveBFS(inicial):
    SI inicial == objetivo ENTONCES retornar 0
    
    q.push(inicial, 0)
    visited.insert(inicial)
    
    MIENTRAS q no vacía HACER
        (actual, costo) = q.front()
        q.pop()
        
        PARA CADA vecino EN generateMoves(actual) HACER
            SI vecino no visitado ENTONCES
                SI vecino == objetivo ENTONCES
                    retornar costo + 1
                FIN SI
                
                q.push(vecino, costo + 1)
                visited.insert(vecino)
            FIN SI
        FIN PARA
    FIN MIENTRAS
    
    retornar -1  // No hay solución
FIN Función
```

**Complejidad:** O(b^d) donde b≈3 (factor ramificación promedio)

#### 4.2.2 A* con h1 (`h1_puzzle_solver.cpp`)

**Estructuras de datos:**
```cpp
struct Node {
    string board;
    int g;  // Costo real
    int f;  // f = g + h
};

priority_queue<Node> pq;              // Min-heap por f
unordered_set<string> closed;         // Lista cerrada
```

**Heurística h1:**
```cpp
int heuristic_h1(const string& board) {
    int misplaced = 0;
    for (int i = 0; i < 16; i++) {
        if (board[i] != '#' && board[i] != TARGET[i]) {
            misplaced++;
        }
    }
    return misplaced;
}
```

#### 4.2.3 A* con h2 (`h2_puzzle_solver.cpp`)

**Heurística h2:**
```cpp
int heuristic_h2(const string& board) {
    int distance = 0;
    for (int i = 0; i < 16; i++) {
        if (board[i] == '#') continue;
        
        int targetPos = TARGET.find(board[i]);
        int currentRow = i / 4, currentCol = i % 4;
        int targetRow = targetPos / 4, targetCol = targetPos % 4;
        
        distance += abs(currentRow - targetRow) + 
                    abs(currentCol - targetCol);
    }
    return distance;
}
```

**Ventajas sobre h1:**
- Más informada: considera distancia real
- Reduce factor de ramificación efectivo
- Compensa overhead con menos expansiones

### 4.3 Algoritmos Paralelos

#### 4.3.1 BFS Paralelo (`parallel_bfs_openmp.cpp`)

**Estrategia:** Paralelización por niveles con buffers locales

**Diseño clave:**
```cpp
while (!currentLevel.empty()) {
    // 1. Extraer todos los estados del nivel
    vector<string> levelStates;
    while (!currentLevel.empty()) {
        levelStates.push_back(currentLevel.front());
        currentLevel.pop();
    }
    
    // 2. Buffers locales por thread (SIN LOCKS)
    vector<vector<string>> threadResults(numThreads);
    
    // 3. Procesamiento paralelo
    #pragma omp parallel num_threads(numThreads)
    {
        int tid = omp_get_thread_num();
        
        #pragma omp for schedule(dynamic)
        for (int i = 0; i < levelStates.size(); i++) {
            vector<string> neighbors = 
                generateMoves(levelStates[i]);
            
            // Guardar en buffer local (SIN LOCK)
            for (auto& n : neighbors) {
                threadResults[tid].push_back(n);
            }
        }
    } // Barrera implícita
    
    // 4. Merge secuencial
    for (int t = 0; t < numThreads; t++) {
        for (auto& neighbor : threadResults[t]) {
            if (!visited.count(neighbor)) {
                currentLevel.push(neighbor);
                visited.insert(neighbor);
            }
        }
    }
}
```

**Ventajas:**
- ✅ Sin locks en fase de expansión
- ✅ Minimiza contención
- ✅ Merge una vez por nivel
- ✅ Escalabilidad superior

#### 4.3.2 A* Paralelo (`parallel_astar_h2_openmp.cpp`)

**Estrategia:** Batch processing con secciones críticas

**Diseño:**
```cpp
while (!openList.empty()) {
    // 1. Extraer batch
    int batchSize = min(numThreads * 4, openList.size());
    vector<Node> batch;
    for (int i = 0; i < batchSize; i++) {
        batch.push_back(openList.top());
        openList.pop();
    }
    
    // 2. Procesamiento paralelo
    #pragma omp parallel num_threads(numThreads)
    {
        #pragma omp for schedule(dynamic)
        for (int i = 0; i < batch.size(); i++) {
            Node current = batch[i];
            
            // CRITICAL: closed list no es thread-safe
            #pragma omp critical(closed_check)
            {
                if (!closedList.count(current.board)) {
                    closedList.insert(current.board);
                }
            }
            
            // Expandir vecinos...
        }
    }
}
```

**Desafíos:**
- ⚠️ Priority queue requiere sincronización
- ⚠️ Closed list requiere secciones críticas
- ⚠️ Mayor overhead por contención

---

## 5. Arquitectura del Sistema

### 5.1 Diagrama de Componentes

```
┌─────────────────────────────────────────────────────────┐
│                    Sistema 15-Puzzle                    │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌───────────────┐  ┌──────────────┐  ┌─────────────┐  │
│  │  Utilidades   │  │  Algoritmos  │  │  Benchmark  │  │
│  │               │  │              │  │             │  │
│  │ • board_moves │  │ • BFS        │  │ • Medición  │  │
│  │ • printer     │  │ • A*-h1      │  │ • Análisis  │  │
│  │ • available   │  │ • A*-h2      │  │ • Gráficos  │  │
│  └───────┬───────┘  └──────┬───────┘  └──────┬──────┘  │
│          │                 │                 │         │
│          └─────────────────┴─────────────────┘         │
│                           │                            │
│          ┌────────────────┴───────────────┐            │
│          │                                │            │
│  ┌───────▼────────┐              ┌───────▼────────┐   │
│  │   Secuencial   │              │    Paralelo    │   │
│  │                │              │    (OpenMP)    │   │
│  │ • bsp_solver   │              │ • parallel_bfs │   │
│  │ • h1_solver    │              │ • parallel_h1  │   │
│  │ • h2_solver    │              │ • parallel_h2  │   │
│  └────────────────┘              └────────────────┘   │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### 5.2 Flujo de Ejecución

```
Input: puzzle.txt
     │
     ▼
[Validación]
     │
     ├─→ [BFS Secuencial] ──→ [Resultados]
     │
     ├─→ [A*-h1 Secuencial] ──→ [Resultados]
     │
     ├─→ [A*-h2 Secuencial] ──→ [Resultados]
     │
     ├─→ [BFS Paralelo] ──→ [Resultados + Speedup]
     │
     ├─→ [A*-h1 Paralelo] ──→ [Resultados + Speedup]
     │
     └─→ [A*-h2 Paralelo] ──→ [Resultados + Speedup]
          │
          ▼
     [Análisis Comparativo]
          │
          ▼
     [Gráficos y Reportes]
```

---

## 6. Resultados Experimentales

### 6.1 Resultados Secuenciales

| Algoritmo | Tiempo Promedio (ms) | Nodos Expandidos | Movimientos | Observaciones |
|-----------|---------------------|------------------|-------------|---------------|
| BFS       | 17.64               | ~8500            | Óptimo      | Garantiza solución óptima |
| A*-h1     | 23.30               | ~6200            | Óptimo      | Overhead > beneficio para puzzles simples |
| **A*-h2** | **12.79**           | **~2800**        | Óptimo      | **Más eficiente** |

**Análisis:**
- A*-h2 es **27.5% más rápido** que BFS
- A*-h2 es **45.1% más rápido** que A*-h1
- A*-h2 expande **67% menos nodos** que BFS
- h1 es ineficiente para puzzles simples (overhead > beneficio)

### 6.2 Resultados Paralelos (4 threads)

| Algoritmo | Tiempo (ms) | Speedup | Eficiencia | Overhead |
|-----------|-------------|---------|------------|----------|
| BFS       | 7.35        | 2.40x   | 60.0%      | Bajo     |
| A*-h1     | 10.43       | 2.23x   | 55.8%      | Medio    |
| A*-h2     | 6.12        | 2.09x   | 52.3%      | Alto     |

**Análisis:**
- BFS paralelo logra el **mejor speedup** (2.40x)
- A*-h2 paralelo es el **más rápido en tiempo absoluto** (6.12 ms)
- Eficiencia del 52-60% es esperada debido a:
  - Overhead de sincronización
  - Fracción secuencial del código
  - Granularidad de las tareas

### 6.3 Escalabilidad

| Threads | BFS Speedup | A*-h2 Speedup | Ideal |
|---------|-------------|---------------|-------|
| 1       | 1.00x       | 1.00x         | 1.00x |
| 2       | 1.82x       | 1.65x         | 2.00x |
| 4       | 2.40x       | 2.09x         | 4.00x |
| 8       | 2.99x       | 2.45x         | 8.00x |

**Observaciones:**
- Speedup sublineal esperado (Ley de Amdahl)
- BFS escala mejor que A* debido a menor contención
- Rendimiento decreciente con >4 threads (overhead > beneficio)

---

## 7. Análisis de Rendimiento

### 7.1 Comparación de Estrategias de Sincronización

#### 7.1.1 BFS: Buffers Locales
```cpp
// Cada thread escribe en su propio buffer
threadResults[tid].push_back(neighbor);  // SIN LOCK
```

**Ventajas:**
- ✅ Cero contención durante expansión
- ✅ Excelente localidad de caché
- ✅ Merge secuencial predecible

**Resultado:** Mejor speedup (2.40x)

#### 7.1.2 A*: Secciones Críticas
```cpp
#pragma omp critical(closed_check)
{
    closedList.insert(current.board);  // CON LOCK
}
```

**Desventajas:**
- ⚠️ Serialización parcial
- ⚠️ Threads esperan por lock
- ⚠️ Mayor overhead

**Resultado:** Menor speedup (2.09x)

### 7.2 Validación con Ley de Amdahl

Asumiendo fracción secuencial s = 0.22 (22%):

```
Speedup_max(8) = 1 / (0.22 + 0.78/8)
                = 1 / (0.22 + 0.0975)
                = 1 / 0.3175
                = 3.15x
```

**Resultados observados:** 2.99x  
**Eficiencia respecto a máximo teórico:** 94.9%

**Conclusión:** Los resultados son consistentes con la teoría.

### 7.3 Análisis de Overhead

| Componente | BFS | A* |
|------------|-----|-----|
| Sincronización | Bajo (merge 1x por nivel) | Alto (critical cada expansión) |
| Gestión de memoria | Media (buffers temporales) | Media (priority queue) |
| Balanceo de carga | Bueno (schedule dynamic) | Regular (batch processing) |
| Localidad de caché | Excelente | Regular |

---

## 8. Conclusiones

### 8.1 Hallazgos Principales

1. **Algoritmos Secuenciales:**
   - A*-h2 es el algoritmo más eficiente con 12.79 ms promedio
   - La calidad de la heurística es más importante que la velocidad de cálculo
   - h1 no compensa su overhead para puzzles simples

2. **Paralelización:**
   - Speedups de 2.09x-2.40x con 4 threads son significativos
   - BFS paralelo logra mejor eficiencia (60%) por diseño sin locks
   - Secciones críticas en A* limitan la escalabilidad

3. **Diseño de Sistemas Paralelos:**
   - Minimizar sincronización es crucial para rendimiento
   - Buffers locales > secciones críticas compartidas
   - Balance entre granularidad y overhead determina eficiencia

4. **Validación Teórica:**
   - Resultados consistentes con Ley de Amdahl (94.9% precisión)
   - Speedup sublineal esperado y explicable
   - Fracción secuencial estimada en 22%

### 8.2 Contribuciones

1. Implementación completa de 6 algoritmos (3 secuenciales + 3 paralelos)
2. Análisis comparativo exhaustivo de estrategias de sincronización
3. Validación experimental de modelos teóricos
4. Código genérico extensible a tableros NxN
5. Documentación técnica completa

### 8.3 Limitaciones

1. **Escalabilidad:** Rendimiento decreciente con >4 threads
2. **Granularidad:** Tareas pequeñas limitan paralelización efectiva
3. **Memoria compartida:** OpenMP limitado a un solo nodo
4. **Complejidad factorial:** NxN con N>5 computacionalmente intratable

---

## 9. Trabajo Futuro

### 9.1 Optimizaciones Inmediatas

1. **Estructuras Lock-Free:**
   - Implementar colas sin locks (lock-free queues)
   - Usar atomic operations para closed list
   - Reducir contención en A*

2. **Pattern Databases:**
   - Precalcular distancias para sub-problemas
   - Heurísticas más informadas
   - Reducir factor de ramificación

3. **Algoritmos Avanzados:**
   - IDA* (Iterative Deepening A*)
   - Parallel IDA* con stack splitting
   - Bidirectional search

### 9.2 Escalabilidad

1. **MPI (Message Passing Interface):**
   - Distribución entre múltiples nodos
   - Paralelización a mayor escala
   - Trabajo coordinado entre clusters

2. **GPU (CUDA/OpenCL):**
   - Exploración masivamente paralela
   - Miles de threads simultáneos
   - Ideal para BFS con gran factor de ramificación

3. **Híbrido OpenMP + MPI:**
   - Paralelización multinivel
   - Escalabilidad a supercomputadoras

### 9.3 Aplicaciones

1. **Robótica:** Planificación de trayectorias
2. **Juegos:** AI para puzzle games
3. **Optimización:** Problemas combinatorios
4. **Educación:** Herramienta didáctica

---

## 10. Referencias

### 10.1 Documentación Técnica

6. OpenMP Architecture Review Board. (2018). "OpenMP Application Programming Interface Version 5.0". https://www.openmp.org/specifications/

7. ISO/IEC 14882:2011. "Information technology — Programming languages — C++". International Organization for Standardization.

8. Ratcliff, D., & Gottlieb, A. (1996). "A Parallel 15-Puzzle Solver". Tech Report, New York University.

### 10.2 Recursos en Línea

9. "The 15-Puzzle Problem". Stanford CS106B Lecture Notes.

10. "Parallel A* Search". Carnegie Mellon University - Parallel Algorithms Course.

---

## 11. Anexos

### 11.1 Compilación

#### Versiones Secuenciales:
```bash
g++ -std=c++11 -O2 -o bsp bsp_puzzle_solver.cpp
g++ -std=c++11 -O2 -o h1 h1_puzzle_solver.cpp
g++ -std=c++11 -O2 -o h2 h2_puzzle_solver.cpp
```

#### Versiones Paralelas:
```bash
g++ -std=c++11 -O2 -fopenmp -o pbfs parallel_bfs_openmp.cpp
g++ -std=c++11 -O2 -fopenmp -o ph1 parallel_astar_h1_openmp.cpp
g++ -std=c++11 -O2 -fopenmp -o ph2 parallel_astar_h2_openmp.cpp
```

### 11.2 Ejecución

#### Secuencial:
```bash
echo "ABCDEFG#IJKHMNOL" | ./h2
```

#### Paralelo:
```bash
echo "ABCDEFG#IJKHMNOL" | ./ph2 4
```

### 11.3 Estructura de Archivos

```
Parcial_Paralela2025_2/
├── README.md                          # Este documento
├── ARCHIVOS_PARA_VIDEO.md             # Lista para grabación
├── GUION_ULTRA_DETALLADO_15MIN.md     # Script de video
├── presentacion_video.tex             # Diapositivas LaTeX
│
├── board_moves.cpp                    # Generación de movimientos
├── board_printer.cpp                  # Visualización
├── board_available.cpp                # Validación
│
├── bsp_puzzle_solver.cpp              # BFS secuencial
├── h1_puzzle_solver.cpp               # A*-h1 secuencial
├── h2_puzzle_solver.cpp               # A*-h2 secuencial
│
├── parallel_bfs_openmp.cpp            # BFS paralelo
├── parallel_astar_h1_openmp.cpp       # A*-h1 paralelo
├── parallel_astar_h2_openmp.cpp       # A*-h2 paralelo
│
├── bsp_puzzle_solver_nxn.cpp          # BFS genérico NxN
├── h2_puzzle_solver_nxn.cpp           # A*-h2 genérico NxN
│
├── puzzles.txt                        # Casos de prueba
├── results.txt                        # Resultados
│
├── full_benchmark.sh                  # Benchmark completo
├── build_and_test.ps1                 # Compilación y pruebas
│
├── informe_compacto.tex               # Informe LaTeX
│
└── DOCUMENTACION_REFERENCIA/          # Documentos auxiliares
    ├── README_COMPLETO.md
    ├── GUIA_RAPIDA.md
    └── ...
```

### 11.5 Contacto

**Estudiante:** Rui Lei  


---

