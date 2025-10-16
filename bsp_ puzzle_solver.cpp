/**
 * @file bsp_puzzle_solver.cpp
 * @brief 4x4 Sliding Puzzle Solver using BFS (Breadth-First Search)
 * 
 * This program solves the 4x4 sliding puzzle by finding the minimum number
 * of moves required to reach the goal state from the initial state.
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

using namespace std;

/**
 * @brief Possible movement directions
 * 
 * Up, Down, Left, Right
 */
const int dRow[] = {-1, 1, 0, 0}; // UP, DOWN, LEFT, RIGHT
const int dCol[] = {0, 0, -1, 1};
const string MOVES[] = {"UP", "DOWN", "LEFT", "RIGHT"};

struct State{
      string board;
      int blankPos;
      int cost;
      // Constructor
      State(string b, int pos, int c) : board(b), blankPos(pos), cost(c) {}
};

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

/**
 * @brief Swaps two tiles on the board and returns new board state
 * 
 */
string swapBoardTiles(const string &currentBoard, int position1, int position2){
      string newBoard = currentBoard;
      swap(newBoard[position1], newBoard[position2]);
      return newBoard;
}

/**
 * @brief Breadth-First Search to find shortest path to goal state
 * 
 * Explores all possible states level by level, guaranteeing the shortest path
 * will be found first due to BFS properties.
 */
int bfs(string start){
      const string TARGET = "ABCDEFGHIJKLMNO#";
      queue<State> q;
      unordered_set<string> visited;

      // Find the initial position of the blank tile ('#')
      int blankPos = start.find('#');
      q.push(State(start, blankPos, 0));
      visited.insert(start);

      while (!q.empty()){
            State current = q.front();
            q.pop();
            
            // Check if goal state is reached
            if (current.board == TARGET) return current.cost;

            // Convert 1D position to 2D coordinates
            int row = current.blankPos / 4;
            int col = current.blankPos % 4;

            // Try all 4 possible moves
            for (int i = 0; i < 4; i++){
                  int newRow = row + dRow[i];
                  int newCol = col + dCol[i];

                  // Check if the move is within bounds
                  if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4){
                        // Convert 2D coordinates back to 1D position
                        int newPos = newRow * 4 + newCol;
                        string newBoard = swapBoardTiles(current.board, current.blankPos, newPos);

                        // If this state hasn't been visited, add it to the queue
                        if (visited.find(newBoard) == visited.end()){
                              q.push(State(newBoard, newPos, current.cost + 1));
                              visited.insert(newBoard);
                        }
                  }
            }
      }
      // No solution found
      return -1;
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main(){
      string start;
      cin >> start;
      int result = bfs(start);
      cout << result << endl;
      return 0;
}