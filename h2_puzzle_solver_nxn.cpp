/**
 * @file h2_puzzle_solver_nxn.cpp
 * @brief NxN Sliding Puzzle Solver using A* with Manhattan Distance (h2)
 * 
 * Generic implementation supporting variable board sizes (3x3, 4x4, 5x5, 8x8, etc.)
 * 
 * Heuristic h2: Manhattan Distance
 *   - Sum of distances each tile is from its goal position
 *   - More informed than h1, typically faster
 *   - Admissible: never overestimates the cost
 * 
 * Usage:
 *   h2_puzzle_solver_nxn <N> <initial_state>
 *   Example: h2_puzzle_solver_nxn 4 ABCDEFG#IJKHMNOL
 * 
 * @author JAPeTo
 * @version 2.0 - Generic NxN support (Tarea No. 7)
 */

#include <iostream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

int N = 4;  // Board size (NxN)
string TARGET;  // Goal state
unordered_map<char, int> targetPositions;  // Cache for target positions

const int dRow[] = {-1, 1, 0, 0};
const int dCol[] = {0, 0, -1, 1};

// =============================================================================
// STATE STRUCTURE
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

/**
 * @brief Generates target state for NxN board
 */
string generateTarget(int n) {
      string target = "";
      int size = n * n;
      
      for (int i = 0; i < size - 1; i++) {
            if (i < 26) {
                  target += (char)('A' + i);
            } else {
                  target += "A" + to_string(i - 25);
            }
      }
      target += '#';
      
      return target;
}

/**
 * @brief Builds cache of target positions for each tile
 */
void buildTargetPositions() {
      targetPositions.clear();
      for (int i = 0; i < (int)TARGET.length(); i++) {
            targetPositions[TARGET[i]] = i;
      }
}

/**
 * @brief Swaps two positions in the board
 */
string swapTiles(const string& board, int pos1, int pos2) {
      string newBoard = board;
      swap(newBoard[pos1], newBoard[pos2]);
      return newBoard;
}

/**
 * @brief Calculates Manhattan Distance heuristic for NxN board
 * @param board Current board state
 * @return Sum of Manhattan distances for all tiles
 */
int calculateH2(const string& board) {
      int distance = 0;
      
      for (int i = 0; i < (int)board.length(); i++) {
            char tile = board[i];
            if (tile == '#') continue;
            
            // Get target position for this tile
            int targetPos = targetPositions[tile];
            
            // Convert positions to 2D coordinates (GENERIC)
            int currentRow = i / N;
            int currentCol = i % N;
            int targetRow = targetPos / N;
            int targetCol = targetPos % N;
            
            // Manhattan distance
            distance += abs(currentRow - targetRow) + abs(currentCol - targetCol);
      }
      
      return distance;
}

// =============================================================================
// A* ALGORITHM WITH H2
// =============================================================================

/**
 * @brief A* algorithm with Manhattan Distance heuristic
 * @param start Initial board state
 * @return Minimum number of moves, or -1 if unsolvable
 */
int astar_h2(string start) {
      priority_queue<State, vector<State>, greater<State>> pq;
      unordered_set<string> visited;
      unordered_map<string, int> bestCost;
      
      int blankPos = start.find('#');
      int h = calculateH2(start);
      
      pq.push(State(start, blankPos, 0, h));
      bestCost[start] = 0;
      
      int nodesExpanded = 0;
      
      while (!pq.empty()) {
            State current = pq.top();
            pq.pop();
            
            // Skip if already visited with better cost
            if (visited.count(current.board)) continue;
            visited.insert(current.board);
            
            nodesExpanded++;
            
            // Check if goal state is reached
            if (current.board == TARGET) {
                  //cerr << "Nodes expanded: " << nodesExpanded << endl;
                  return current.cost;
            }
            
            // Convert 1D position to 2D coordinates (GENERIC)
            int row = current.blankPos / N;
            int col = current.blankPos % N;
            
            // Try all 4 possible moves
            for (int i = 0; i < 4; i++) {
                  int newRow = row + dRow[i];
                  int newCol = col + dCol[i];
                  
                  // Validate bounds (GENERIC)
                  if (newRow >= 0 && newRow < N && newCol >= 0 && newCol < N) {
                        int newPos = newRow * N + newCol;
                        string newBoard = swapTiles(current.board, current.blankPos, newPos);
                        int newCost = current.cost + 1;
                        
                        // Only add if better cost
                        if (!bestCost.count(newBoard) || newCost < bestCost[newBoard]) {
                              bestCost[newBoard] = newCost;
                              int h = calculateH2(newBoard);
                              pq.push(State(newBoard, newPos, newCost, h));
                        }
                  }
            }
      }
      
      return -1; // No solution found
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main(int argc, char* argv[]) {
      if (argc < 2) {
            // Default mode: read from stdin for compatibility
            string puzzle;
            cin >> puzzle;
            
            // Auto-detect board size
            int len = puzzle.length();
            N = 4;  // Default to 4x4
            
            if (len == 9) N = 3;
            else if (len == 16) N = 4;
            else if (len == 25) N = 5;
            else if (len == 64) N = 8;
            
            TARGET = generateTarget(N);
            buildTargetPositions();
            
            auto startTime = high_resolution_clock::now();
            int solution = astar_h2(puzzle);
            auto endTime = high_resolution_clock::now();
            
            double timeMs = duration_cast<microseconds>(endTime - startTime).count() / 1000.0;
            
            cout << solution << endl;
            //cerr << "Time: " << timeMs << " ms" << endl;
            
      } else if (argc == 3) {
            // New mode: specify board size
            N = atoi(argv[1]);
            string puzzle = argv[2];
            
            if (N < 2 || N > 10) {
                  cerr << "Error: Board size must be between 2 and 10" << endl;
                  return 1;
            }
            
            if ((int)puzzle.length() != N * N) {
                  cerr << "Error: Puzzle length must be " << N*N << " for " << N << "x" << N << " board" << endl;
                  return 1;
            }
            
            TARGET = generateTarget(N);
            buildTargetPositions();
            
            cout << "========================================" << endl;
            cout << "A* Solver (h2) - " << N << "x" << N << " Puzzle" << endl;
            cout << "========================================" << endl;
            cout << "Initial: " << puzzle << endl;
            cout << "Target:  " << TARGET << endl;
            cout << "Heuristic: Manhattan Distance (h2)" << endl;
            cout << "----------------------------------------" << endl;
            
            auto startTime = high_resolution_clock::now();
            int solution = astar_h2(puzzle);
            auto endTime = high_resolution_clock::now();
            
            double timeMs = duration_cast<microseconds>(endTime - startTime).count() / 1000.0;
            
            cout << "Solution: " << solution << " moves" << endl;
            cout << "Time: " << timeMs << " ms" << endl;
            cout << "========================================" << endl;
            
      } else {
            cerr << "Usage: " << argv[0] << " [board_size] <puzzle>" << endl;
            cerr << "  Mode 1 (auto-detect): echo \"ABCDEFG#IJKHMNOL\" | " << argv[0] << endl;
            cerr << "  Mode 2 (specify size): " << argv[0] << " 4 ABCDEFG#IJKHMNOL" << endl;
            cerr << endl;
            cerr << "Examples:" << endl;
            cerr << "  3x3 (8-puzzle):   " << argv[0] << " 3 ABC#EFGHI" << endl;
            cerr << "  4x4 (15-puzzle):  " << argv[0] << " 4 ABCDEFG#IJKHMNOL" << endl;
            cerr << "  5x5 (24-puzzle):  " << argv[0] << " 5 ABCDEFGHIJ#LMNOPQRSTUVWXY" << endl;
            return 1;
      }
      
      return 0;
}
