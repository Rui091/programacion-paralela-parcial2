/**
 * @file board_available.cpp
 * @brief 4x4 Sliding Puzzle Available Moves Finder
 * 
* This program analyzes a 4x4 sliding puzzle board configuration and determines
 * all valid moves that can be made from the current state.
 */
#include <iostream>
#include <string>

using namespace std;

/**
 * @brief Finds and displays all available moves for the current board state
 *
 */
void listAvailable(const string &board){
  // Find position of '#' in the string (16 characters, 4x4 board)
  int blankPos = board.find('#');
  
  // Convert 1D position to 2D coordinates
  int row = blankPos / 4;
  int col = blankPos % 4;
  
  // Check and print available moves in order: UP, DOWN, LEFT, RIGHT
  if (row > 0) cout << "UP" << endl;      // Can move up if not in first row
  if (row < 3) cout << "DOWN" << endl;    // Can move down if not in last row
  if (col > 0) cout << "LEFT" << endl;    // Can move left if not in first column
  if (col < 3) cout << "RIGHT";           // Can move right if not in last column (no endl on last)
}
/**
 * @brief Main function - program entry point
 * 
 * Reads the board configuration from standard input and displays all
 * available moves based on the empty space position.
 */
int main(){
      string board;
      cin >> board;
      listAvailable(board);
      return 0;
}