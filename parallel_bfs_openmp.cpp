/**
 * @file parallel_bfs_openmp.cpp
 * @brief Parallel BFS implementation using OpenMP
 * 
 * This program implements a parallel version of BFS using OpenMP for shared-memory
 * parallelism with work distribution across threads. Uses level-by-level exploration
 * with dynamic work distribution.
 * 
 * Compilation:
 *   g++ -std=c++11 -O2 -fopenmp -o parallel_bfs_openmp.exe parallel_bfs_openmp.cpp
 * 
 * Usage:
 *   parallel_bfs_openmp.exe <puzzle> [num_threads]
 * 
 * @author JAPeTo
 * @version 2.0
 */

#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace chrono;

const string TARGET = "ABCDEFGHIJKLMNO#";
const int dRow[] = {-1, 1, 0, 0};
const int dCol[] = {0, 0, -1, 1};

struct State {
      string board;
      int blankPos;
      int cost;
      
      State(string b, int pos, int c) : board(b), blankPos(pos), cost(c) {}
};

string swapTiles(const string& board, int pos1, int pos2) {
      string newBoard = board;
      swap(newBoard[pos1], newBoard[pos2]);
      return newBoard;
}

int parallelBFS(string start, int numThreads) {
      vector<State> currentLevel;
      vector<State> nextLevel;
      unordered_set<string> visited;
      
      int blankPos = start.find('#');
      currentLevel.push_back(State(start, blankPos, 0));
      visited.insert(start);
      
      int nodesExpanded = 0;
      bool foundSolution = false;
      int solution = -1;
      
      while (!currentLevel.empty() && !foundSolution) {
            nextLevel.clear();
            
            #pragma omp parallel num_threads(numThreads)
            {
                  vector<State> localNextLevel;
                  
                  #pragma omp for schedule(dynamic)
                  for (int idx = 0; idx < currentLevel.size(); idx++) {
                        if (foundSolution) continue;
                        
                        State current = currentLevel[idx];
                        
                        #pragma omp atomic
                        nodesExpanded++;
                        
                        // Check if goal is reached
                        if (current.board == TARGET) {
                              #pragma omp critical
                              {
                                    if (!foundSolution) {
                                          foundSolution = true;
                                          solution = current.cost;
                                    }
                              }
                              continue;
                        }
                        
                        int row = current.blankPos / 4;
                        int col = current.blankPos % 4;
                        
                        // Explore neighbors
                        for (int i = 0; i < 4; i++) {
                              int newRow = row + dRow[i];
                              int newCol = col + dCol[i];
                              
                              if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4) {
                                    int newPos = newRow * 4 + newCol;
                                    string newBoard = swapTiles(current.board, current.blankPos, newPos);
                                    
                                    bool shouldAdd = false;
                                    #pragma omp critical(visited_update)
                                    {
                                          if (visited.find(newBoard) == visited.end()) {
                                                visited.insert(newBoard);
                                                shouldAdd = true;
                                          }
                                    }
                                    
                                    if (shouldAdd) {
                                          localNextLevel.push_back(State(newBoard, newPos, current.cost + 1));
                                    }
                              }
                        }
                  }
                  
                  // Merge local results into global next level
                  #pragma omp critical(nextlevel_update)
                  {
                        nextLevel.insert(nextLevel.end(), localNextLevel.begin(), localNextLevel.end());
                  }
            }
            
            if (foundSolution) break;
            currentLevel = nextLevel;
      }
      
      //cout << "Nodes expanded: " << nodesExpanded << endl;
      return solution;
}

int main(int argc, char* argv[]) {
      string puzzle;
      cin >> puzzle;
      
      int numThreads = (argc >= 2) ? atoi(argv[1]) : omp_get_max_threads();
      
      //cout << "Solving puzzle: " << puzzle << endl;
      //cout << "Using " << numThreads << " threads" << endl;
      
      auto startTime = high_resolution_clock::now();
      int solution = parallelBFS(puzzle, numThreads);
      auto endTime = high_resolution_clock::now();
      
      double timeMs = duration_cast<microseconds>(endTime - startTime).count() / 1000.0;
      
      cout << solution << endl;
      //cout << "Time: " << timeMs << " ms" << endl;
      
      return 0;
}
