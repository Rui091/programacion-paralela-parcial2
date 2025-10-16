/**
 * @file h1_puzzle_solver.cpp
 * @brief 4x4 Sliding Puzzle Solver using A* with h1 heuristic
 * 
 * This program solves the 4x4 sliding puzzle using A* algorithm
 * with h1 heuristic (number of misplaced tiles).
 * 
 * Goal state: "ABCDEFGHIJKLMNO#"
 * Where '#' represents the empty space.
 * 
 * @author JAPeTo
 * @version 1.6
 */
#include <iostream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>

using namespace std;

const string TARGET = "ABCDEFGHIJKLMNO#";
const int dRow[] = {-1, 1, 0, 0}; // UP, DOWN, LEFT, RIGHT
const int dCol[] = {0, 0, -1, 1};

/**
 * @brief State structure for A* search
 */
struct State {
      string board;
      int blankPos;
      int cost;      // g(n) - actual cost from start
      int heuristic; // h(n) - estimated cost to goal
      
      State(string b, int pos, int c, int h) : board(b), blankPos(pos), cost(c), heuristic(h) {}
      
      // f(n) = g(n) + h(n)
      int f() const { return cost + heuristic; }
};

/**
 * @brief Comparator for priority queue (min-heap based on f(n))
 */
struct CompareState {
      bool operator()(const State& a, const State& b) const {
            return a.f() > b.f(); // Min-heap: lower f(n) has higher priority
      }
};

/**
 * @brief Calculates h1 heuristic: number of misplaced tiles (excluding '#')
 */
int calculateH1(const string& board) {
      int misplaced = 0;
      for (int i = 0; i < 16; i++) {
            if (board[i] != '#' && board[i] != TARGET[i]) {
                  misplaced++;
            }
      }
      return misplaced;
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
 * @brief A* search algorithm with h1 heuristic
 */
int aStarSearch(string start){
      priority_queue<State, vector<State>, CompareState> pq;
      unordered_set<string> visited;
      unordered_map<string, int> bestCost; // Track best cost to reach each state
      
      int blankPos = start.find('#');
      int h = calculateH1(start);
      pq.push(State(start, blankPos, 0, h));
      bestCost[start] = 0;
      
      while (!pq.empty()) {
            State current = pq.top();
            pq.pop();
            
            // Check if goal state is reached
            if (current.board == TARGET) return current.cost;
            
            // Skip if we've already visited this state
            if (visited.count(current.board)) continue;
            visited.insert(current.board);
            
            // Convert 1D position to 2D coordinates
            int row = current.blankPos / 4;
            int col = current.blankPos % 4;
            
            // Try all 4 possible moves
            for (int i = 0; i < 4; i++) {
                  int newRow = row + dRow[i];
                  int newCol = col + dCol[i];
                  
                  // Check if the move is within bounds
                  if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4) {
                        int newPos = newRow * 4 + newCol;
                        string newBoard = swapTiles(current.board, current.blankPos, newPos);
                        int newCost = current.cost + 1;
                        
                        // Only add if we haven't found a better path to this state
                        if (!bestCost.count(newBoard) || newCost < bestCost[newBoard]) {
                              bestCost[newBoard] = newCost;
                              int h = calculateH1(newBoard);
                              pq.push(State(newBoard, newPos, newCost, h));
                        }
                  }
            }
      }
      
      // No solution found
      return -1;
}

int main(){
      string start;
      cin >> start;
      int result = aStarSearch(start);
      cout << result << endl;
      return 0;
}