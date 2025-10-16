/**
 * @file bsp_puzzle_solver_nxn.cpp
 * @brief NxN Sliding Puzzle Solver using BFS (Breadth-First Search)
 * 
 * This program solves NxN sliding puzzles (8-puzzle, 15-puzzle, 24-puzzle, etc.)
 * by finding the minimum number of moves required to reach the goal state.
 * 
 * Supports variable board sizes: 3x3, 4x4, 5x5, 8x8, etc.
 * Can use letters (A-Z) or numbers (0-N²-1)
 * 
 * Usage:
 *   bsp_puzzle_solver_nxn <N> <initial_state>
 *   Example: bsp_puzzle_solver_nxn 4 ABCDEFG#IJKHMNOL
 * 
 * @author JAPeTo
 * @version 2.0 - Generic NxN support (Tarea No. 7)
 */

#include <iostream>
#include <queue>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

int N = 4;  // Board size (NxN)
string TARGET;  // Goal state (generated dynamically)

const int dRow[] = {-1, 1, 0, 0}; // UP, DOWN, LEFT, RIGHT
const int dCol[] = {0, 0, -1, 1};

// =============================================================================
// STATE STRUCTURE
// =============================================================================

struct State {
      string board;
      int blankPos;
      int cost;
      
      State(string b, int pos, int c) : board(b), blankPos(pos), cost(c) {}
};

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

/**
 * @brief Generates target state for NxN board
 * @param n Board size
 * @param useNumbers If true, uses numbers (0,1,2...), else uses letters (A,B,C...)
 * @return Target state string
 */
string generateTarget(int n, bool useNumbers = false) {
      string target = "";
      int size = n * n;
      
      if (useNumbers) {
            // Numbers: 0,1,2,3,...,N²-2,# (# represents N²-1)
            for (int i = 0; i < size - 1; i++) {
                  // Pad with zeros for alignment
                  string num = to_string(i);
                  if (n >= 4) {
                        while (num.length() < to_string(size-1).length()) {
                              num = "0" + num;
                        }
                  }
                  target += num;
                  if (i < size - 2) target += ",";
            }
            target += ",#";
      } else {
            // Letters: A,B,C,...,#
            for (int i = 0; i < size - 1; i++) {
                  if (i < 26) {
                        target += (char)('A' + i);
                  } else {
                        // For boards larger than 5x5, use A1, A2, etc.
                        target += "A" + to_string(i - 25);
                  }
            }
            target += '#';
      }
      
      return target;
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
 * @brief Validates if the puzzle is solvable
 * @param board Initial board state
 * @return true if solvable, false otherwise
 */
bool isSolvable(const string& board) {
      // Count inversions
      int inversions = 0;
      int size = N * N;
      
      for (int i = 0; i < size - 1; i++) {
            if (board[i] == '#') continue;
            for (int j = i + 1; j < size; j++) {
                  if (board[j] == '#') continue;
                  if (board[i] > board[j]) inversions++;
            }
      }
      
      // For odd N: solvable if inversions is even
      // For even N: solvable if (inversions + blank row from bottom) is odd
      if (N % 2 == 1) {
            return inversions % 2 == 0;
      } else {
            int blankPos = board.find('#');
            int blankRow = blankPos / N;
            int blankRowFromBottom = N - blankRow;
            return (inversions + blankRowFromBottom) % 2 == 1;
      }
}

// =============================================================================
// BFS ALGORITHM
// =============================================================================

/**
 * @brief Breadth-First Search for NxN puzzle
 * @param start Initial board state
 * @return Minimum number of moves, or -1 if unsolvable
 */
int bfs(string start) {
      queue<State> q;
      unordered_set<string> visited;
      
      int blankPos = start.find('#');
      q.push(State(start, blankPos, 0));
      visited.insert(start);
      
      int nodesExpanded = 0;
      
      while (!q.empty()) {
            State current = q.front();
            q.pop();
            
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
                        
                        if (visited.find(newBoard) == visited.end()) {
                              visited.insert(newBoard);
                              q.push(State(newBoard, newPos, current.cost + 1));
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
            
            // Try to detect size
            if (len == 9) N = 3;       // 3x3 = 9 tiles
            else if (len == 16) N = 4;  // 4x4 = 16 tiles
            else if (len == 25) N = 5;  // 5x5 = 25 tiles
            else if (len == 64) N = 8;  // 8x8 = 64 tiles
            
            TARGET = generateTarget(N, false);
            
            auto startTime = high_resolution_clock::now();
            int solution = bfs(puzzle);
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
            
            TARGET = generateTarget(N, false);
            
            cout << "========================================" << endl;
            cout << "BFS Solver - " << N << "x" << N << " Puzzle" << endl;
            cout << "========================================" << endl;
            cout << "Initial: " << puzzle << endl;
            cout << "Target:  " << TARGET << endl;
            cout << "----------------------------------------" << endl;
            
            if (!isSolvable(puzzle)) {
                  cout << "This puzzle is NOT SOLVABLE" << endl;
                  return 1;
            }
            
            auto startTime = high_resolution_clock::now();
            int solution = bfs(puzzle);
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
