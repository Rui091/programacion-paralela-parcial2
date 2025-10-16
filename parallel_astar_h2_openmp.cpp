/**
 * @file parallel_astar_h2_openmp.cpp
 * @brief Parallel A* with h2 heuristic (Manhattan distance) using OpenMP
 * 
 * This program implements a parallel version of A* algorithm with h2 heuristic
 * (sum of Manhattan distances) using OpenMP for shared-memory parallelism.
 * 
 * Manhattan distance is more informed than h1, typically resulting in fewer
 * nodes expanded and faster convergence to the solution.
 * 
 * Compilation:
 *   g++ -std=c++11 -O2 -fopenmp -o parallel_astar_h2_openmp.exe parallel_astar_h2_openmp.cpp
 * 
 * Usage:
 *   parallel_astar_h2_openmp.exe <puzzle> [num_threads]
 * 
 * Example:
 *   parallel_astar_h2_openmp.exe "EABCM#GDKFILNOJH" 4
 * 
 * @author JAPeTo
 * @version 2.0
 */

#include <iostream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <omp.h>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace chrono;

const string TARGET = "ABCDEFGHIJKLMNO#";
const int dRow[] = {-1, 1, 0, 0};
const int dCol[] = {0, 0, -1, 1};

// =============================================================================
// STRUCTURES
// =============================================================================

struct State {
      string board;
      int blankPos;
      int cost;      // g(n)
      int heuristic; // h(n)
      
      State(string b, int pos, int c, int h) 
            : board(b), blankPos(pos), cost(c), heuristic(h) {}
      
      int f() const { return cost + heuristic; }
      
      bool operator>(const State& other) const {
            return f() > other.f();
      }
};

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

string swapTiles(const string& board, int pos1, int pos2) {
      string newBoard = board;
      swap(newBoard[pos1], newBoard[pos2]);
      return newBoard;
}

/**
 * @brief Calculates h2 heuristic: sum of Manhattan distances
 * 
 * For each tile, calculates the Manhattan distance to its goal position.
 * Manhattan distance = |current_row - goal_row| + |current_col - goal_col|
 */
int calculateH2(const string& board) {
      int totalDistance = 0;
      
      for (int i = 0; i < 16; i++) {
            if (board[i] != '#') {
                  // Find goal position of this tile
                  int goalPos = TARGET.find(board[i]);
                  
                  // Convert positions to 2D coordinates
                  int currentRow = i / 4;
                  int currentCol = i % 4;
                  int goalRow = goalPos / 4;
                  int goalCol = goalPos % 4;
                  
                  // Calculate Manhattan distance
                  totalDistance += abs(currentRow - goalRow) + abs(currentCol - goalCol);
            }
      }
      
      return totalDistance;
}

// =============================================================================
// PARALLEL A* ALGORITHM WITH H2
// =============================================================================

int parallelAStarH2(string start, int numThreads) {
      // Shared data structures
      vector<State> openList;
      unordered_set<string> closedSet;
      unordered_map<string, int> bestCost;
      
      int blankPos = start.find('#');
      int h = calculateH2(start);
      openList.push_back(State(start, blankPos, 0, h));
      bestCost[start] = 0;
      
      bool solutionFound = false;
      int solution = -1;
      int nodesExpanded = 0;
      
      omp_set_num_threads(numThreads);
      
      while (!openList.empty() && !solutionFound) {
            // Sort open list to get states with best f(n) values
            sort(openList.begin(), openList.end(), greater<State>());
            
            // Determine batch size for parallel processing
            int batchSize = min((int)openList.size(), numThreads * 4);
            vector<State> currentBatch(openList.end() - batchSize, openList.end());
            openList.erase(openList.end() - batchSize, openList.end());
            
            vector<State> newStates;
            
            #pragma omp parallel num_threads(numThreads)
            {
                  vector<State> localNewStates;
                  
                  #pragma omp for schedule(dynamic) nowait
                  for (int idx = 0; idx < currentBatch.size(); idx++) {
                        if (solutionFound) continue;
                        
                        State current = currentBatch[idx];
                        
                        // Skip if already in closed set
                        bool inClosed = false;
                        #pragma omp critical(closed_check)
                        {
                              inClosed = closedSet.count(current.board);
                              if (!inClosed) {
                                    closedSet.insert(current.board);
                              }
                        }
                        
                        if (inClosed) continue;
                        
                        #pragma omp atomic
                        nodesExpanded++;
                        
                        // Check if goal is reached
                        if (current.board == TARGET) {
                              #pragma omp critical(solution_update)
                              {
                                    if (!solutionFound || current.cost < solution) {
                                          solutionFound = true;
                                          solution = current.cost;
                                    }
                              }
                              continue;
                        }
                        
                        // Expand neighbors
                        int row = current.blankPos / 4;
                        int col = current.blankPos % 4;
                        
                        for (int i = 0; i < 4; i++) {
                              int newRow = row + dRow[i];
                              int newCol = col + dCol[i];
                              
                              if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4) {
                                    int newPos = newRow * 4 + newCol;
                                    string newBoard = swapTiles(current.board, current.blankPos, newPos);
                                    int newCost = current.cost + 1;
                                    
                                    bool shouldAdd = false;
                                    #pragma omp critical(bestcost_update)
                                    {
                                          if (!bestCost.count(newBoard) || newCost < bestCost[newBoard]) {
                                                bestCost[newBoard] = newCost;
                                                shouldAdd = true;
                                          }
                                    }
                                    
                                    if (shouldAdd) {
                                          int h = calculateH2(newBoard);
                                          localNewStates.push_back(State(newBoard, newPos, newCost, h));
                                    }
                              }
                        }
                  }
                  
                  // Merge local new states into global list
                  #pragma omp critical(openlist_update)
                  {
                        newStates.insert(newStates.end(), localNewStates.begin(), localNewStates.end());
                  }
            }
            
            // Add new states to open list
            openList.insert(openList.end(), newStates.begin(), newStates.end());
      }
      
      //cout << "Nodes expanded: " << nodesExpanded << endl;
      return solution;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main(int argc, char* argv[]) {
      string puzzle;
      cin >> puzzle;
      
      int numThreads = (argc >= 2) ? atoi(argv[1]) : omp_get_max_threads();
      
      //cout << "========================================" << endl;
      //cout << "Parallel A* with h2 (OpenMP)" << endl;
      //cout << "========================================" << endl;
      //cout << "Puzzle: " << puzzle << endl;
      //cout << "Threads: " << numThreads << endl;
      //cout << "Target: " << TARGET << endl;
      //cout << "Heuristic: Manhattan Distance (h2)" << endl;
      //cout << "----------------------------------------" << endl;
      
      auto startTime = high_resolution_clock::now();
      int solution = parallelAStarH2(puzzle, numThreads);
      auto endTime = high_resolution_clock::now();
      
      double timeMs = duration_cast<microseconds>(endTime - startTime).count() / 1000.0;
      
      //cout << "----------------------------------------" << endl;
      cout << solution << endl;
      //cout << "Time: " << timeMs << " ms" << endl;
      //cout << "========================================" << endl;
      
      return 0;
}
