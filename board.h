#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include "disk.h"
using namespace std;

enum GAME_STATE { X_VICTORY, O_VICTORY, TIE, INCOMPLETE };

template <size_t rows = 8, size_t cols = 8> class Board {
public:
  Disk board[rows][cols];

  // height of each column in disks, i.e. number of disks added to each column
  int columnHeights[cols] = {};

  // checks for a tie, ASSUMING there was no victory
  // it will return true even if the last disk added results in a victory
  bool checkTie() {
    // check to see if board is full
    for (int i = 0; i < cols; ++i) {
      if (columnHeights[i] != rows)
        return false;
    }
    return true;
  }

  GAME_STATE getState() {
    GAME_STATE winState = checkWin();
    if (winState == X_VICTORY || winState == O_VICTORY)
      return winState;
    if (checkTie())
      return TIE;
    return INCOMPLETE;
  }

  void display(ostream &out = cout) {
    for (int i = rows - 1; i >= 0; i--) {
      for (int j = 0; j < cols; j++) {
        out << board[i][j] << "|";
      }
      out << '\n';
      for (int j = 0; j < cols; j++) {
        out << "--";
      }
      out << '\n';
    }
    // number the columns
    for (int i = 1; i <= cols; ++i) {
      out << i << ' ';
    }
    out << '\n';
  }

  // checks to see if there's a diagonal of 3 matching disks above right the
  // current disk
  bool checkUpperRightDiagonal(Disk disk, int row, int col) {
    if (col + 3 >= cols)
      return false;
    if (row + 3 >= rows)
      return false;
    for (int offset = 1; offset <= 3; ++offset) {
      if (board[row + offset][col + offset] != disk)
        return false;
    }
    return true;
  }

  // checks to see if there's a diagonal of 3 matching disks above left the
  // current disk
  bool checkUpperLeftDiagonal(Disk disk, int row, int col) {
    if (col - 3 < 0)
      return false;
    if (row + 3 >= rows)
      return false;
    for (int offset = 1; offset <= 3; ++offset) {
      if (board[row + offset][col - offset] != disk)
        return false;
    }
    return true;
  }

  // checks to see if there's a diagonal of 3 matching disks above left the
  // current disk
  bool checkUpperDiagonal(Disk disk, int row, int col) {
    return checkUpperLeftDiagonal(disk, row, col) ||
           checkUpperRightDiagonal(disk, row, col);
  }

  // checks to see if there's a vertical chain of 3 matching disks above the
  // current disk
  bool checkVertical(Disk disk, int row, int col) {
    // if there aren't enough rows above
    if ((row + 3) >= rows)
      return false;
    // check 3 disks above
    for (int i = row + 1; i <= row + 3; ++i) {
      // if they don't match, there's no chain
      if (board[i][col] != disk)
        return false;
    }
    // there was a chain
    return true;
  }

  // checks to see if there's a horizontal chain of 3 matching disks right of
  // the current disk
  bool checkHorizontal(Disk disk, int row, int col) {
    // if there aren't enough disks to the right
    if ((col + 3) >= cols)
      return false;
    // check the 3 disks to the right to see if they match
    for (int i = col + 1; i <= col + 3; ++i) {
      // if they don't match, there's no chain
      if (board[row][i] != disk)
        return false;
    }
    // there was a chain
    return true;
  }

  // returns X_VICTORY, O_VICTORY, or INCOMPLETE
  GAME_STATE checkWin() {
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {

        Disk disk;
        disk = board[i][j];
        if (disk.type != EMPTY) {
          if (checkVertical(disk, i, j) or checkHorizontal(disk, i, j) or
              checkUpperLeftDiagonal(disk, i, j) or
              checkUpperRightDiagonal(disk, i, j)) {

            switch (disk.type) {
            case X:
              return X_VICTORY;
            case O:
              return O_VICTORY;
            }
          }
        }
      }
    }
    return INCOMPLETE;
  }

  // returns whether a disk can be added to the column
  bool validMove(int col) {
    if (col < 0 || col >= cols) {
      // input/output should be handled outside of Board class
      // for modularity
      // cout << "Invalid column. Please choose a valid column. \n";
      return false;
    }

    int row = columnHeights[col];

    // rows are zero-indexed, number of rows is not a valid index
    if (row >= rows) {
      // cout << "Column is full. Please choose a different column. \n";
      return false;
    }

    return true;
  }

  // returns whether adding the disk was successful
  bool addDisk(Disk disk, int col) {

    if (validMove(col)) {
      int row = columnHeights[col];
      board[row][col] = disk;
      columnHeights[col]++;
      return true;
    }
    return false;
  }

  // removes the first disk in the column
  void popDisk(int col) {
    int row = columnHeights[col] - 1;
    board[row][col] = EMPTY;
    columnHeights[col]--;
  }

  Disk getDisk(unsigned char row, unsigned char col) const {
    return board[row][col];
  }
};


#endif /* BOARD_H */
