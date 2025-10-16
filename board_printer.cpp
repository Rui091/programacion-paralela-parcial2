/**
 * @file board_printer.cpp
 * @brief 4x4 Board Initialization and Printing Utility
 * 
 * This program reads a 16-character string representing a 4x4 board configuration
 * and converts it into a 2D grid format for display. The input string is interpreted
 * as row-major order (left to right, top to bottom).
 * 
 * Input format: 16 consecutive characters (no spaces)
 * Output format: 4x4 grid with space-separated columns and newline-separated rows
 * 
 * Example:
 *   Input:  "ABCDEFGHIJKLMNO#"
 *   Output: A B C D
 *           E F G H
 *           I J K L
 *           M N O #
 * 
 * @author JAPeTo
 * @version 1.0
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
 * Reads a 16-character input string, converts it to a 4x4 board representation,
 * Compilation: 
 *      g++ -std=c++11 -o board_printer board_printer.cpp
 * 
 * # Linux/Mac/WSL
 * echo "ABCDEFGHIJKLMNO#" | ./board_printer
 * 
 * # Windows Command Prompt
 * echo ABCDEFGHIJKLMNO# | board_printer.exe
 * 
 * # Windows PowerShell
 * "ABCDEFGHIJKLMNO#" | .\board_printer.exe
 * 
 */
int main(){
      vector<vector<char>> board;
      string in;
      cin >> in;

      for (int r = 0; r < 4; ++r){
            vector<char> row;
            for (int i = 0; i < 4; i++) {
                  row.push_back(in[4 * r + i]);
            }
            board.push_back(row);
      }
      print_board(board);
      return 0;
}