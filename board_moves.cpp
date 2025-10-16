/**
 * @file board_moves.cpp
 * @brief 4x4 Sliding Puzzle Move Simulator
 */
#include <iostream>
#include <vector>

using namespace std;

/**
 * @brief Prints a 4x4 board with proper formatting
 */
void print_board(vector<vector<char>> board){
      for (int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++){
                  cout << board[i][j];
                  if (j < 3) cout << " ";
            }
            if (i < 3) cout << endl;
      }
}
/**
 * @brief Executes a move on the puzzle board by sliding the empty space
 * 
 * Searches for the empty space ('#') and attempts to move it in the specified
 * direction by swapping with the adjacent tile. The move is only executed if
 * it remains within the board boundaries.
 */
void doMove(vector<vector<char>> &board, string move){
      for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
          if (board[i][j] == '#'){
            if (move == "DOWN" && i < 3){
              char temp = board[i + 1][j];
              board[i][j] = temp;
              board[i + 1][j] = '#';
              break;
            }else if (move == "UP" && i > 0){
              char temp = board[i - 1][j];
              board[i][j] = temp;
              board[i - 1][j] = '#';
              break;
            }else if (move == "RIGHT" && j < 3){
              char temp = board[i][j + 1];
              board[i][j] = temp;
              board[i][j + 1] = '#';
              break;
            }else if (move == "LEFT" && j > 0){
              char temp = board[i][j - 1];
              board[i][j] = temp;
              board[i][j - 1] = '#';
              break;
            }
          }
        }
      }
      print_board(board);
}

/**
 * @brief Main function - program entry point
 * 
 * Reads input, initializes the board, and executes the requested move.
 */
int main(){
      vector<vector<char>> board;
      string in;
      string move;
      cin >> in;
      cin >> move;

      for (int r = 0; r < 4; ++r){
            vector<char> row;
            for (int i = 0; i < 4; i++)
            {
                  row.push_back(in[4 * r + i]);
            }
            board.push_back(row);
      }
      doMove(board, move);
      return 0;
}