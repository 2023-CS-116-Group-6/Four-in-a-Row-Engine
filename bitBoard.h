#ifndef BITBOARD_H
#define BITBOARD_H

#include <bitset>
#include <sstream>
#include <string>
#include "disk.h"
#include "board.h"
using namespace std;

// class to save time checking for win/loss by using bitsets
// https://en.wikipedia.org/wiki/Bitboard
class BitBoard {
public:
  // constructs empty BitBoard
  BitBoard() {}
  // constructs a BitBoard from a normal board
  BitBoard(const Board<8, 8> &inefficientBoard) {
    for (int col = 0; col < 8; ++col) {
      // columnHeights[col] = inefficientBoard.columnHeights[col];
      for (int row = 0; row < inefficientBoard.columnHeights[col]; ++row) {
        Disk disk = inefficientBoard.getDisk(row, col);
        addDisk(disk, col);
        // addDisk(getLocations(disk), row, col);
      }
    }
  }
  // check if there's any 4-in-a-row diagonally
  bool checkDiagonal(const bitset<64> &locations) const {
    bitset<64> rightBits = locations;
    // right diagonals are spaced 9 apart,
    // each 1 bit means there was 2 right diagonal in a row starting there
    // right shifts destroy bits at lower indices, left shifts destroy bits at
    // higher indices https://en.wikipedia.org/wiki/Bitboard#Other_games
    rightBits &= rightBits >> 9;
    // if two diagonals above is 1, then there's a diagonal of 4
    rightBits &= rightBits >> 18;
    if (rightBits.any())
      return true;

    bitset<64> leftBits = locations;
    // left diagonals are spaced 7 apart
    leftBits &= leftBits >> 7;
    leftBits &= leftBits >> 14;
    return leftBits.any();
  }

  // creates a bit mask by concatenating rowExclusionBits 8 times
  static constexpr bitset<64> rowMask(unsigned char rowExclusionBits) {
    unsigned long long exclusionBits = rowExclusionBits;
    for (int i = 0; i < 7; ++i)
      exclusionBits |= exclusionBits << 8;
    unsigned long long inclusionBits = ~exclusionBits;
    return inclusionBits;
  }

  // requires at least C++14 for constexpr
  // mask to exclude locations that cannot have a right horizontal 4-in-a-row
  static constexpr bitset<64> horizontal4ChainMask() {
    /* contains bits to exclude from horizontal checks
     * the rightmost columns are actually represented
     * by the leftmost bits in the bitstring :(
     */
    // every column from 6 on up can't have a right horizontal chain
    return rowMask(0b11100000);
  }

  // check if there's any 4-in-a-row horizontally
  bool checkHorizontal(const bitset<64> &locations) const {
    bitset<64> horizontalBits = locations;
    // each 1-bit means there were 2 in a row horizontally, except near the end
    // of a row
    horizontalBits &= horizontalBits >> 1;
    // if there was a 2-chain 2 columns over and a 2-chain here, then there's a
    // 4-chain
    horizontalBits &= horizontalBits >> 2;
    // horizontal 4-chains don't exist beginning at the 6-th column
    horizontalBits &= horizontal4ChainMask();
    // any 1-bit means there's a horizontal chain
    return horizontalBits.any();
  }
  // check if there's any 4-in-a-row vertically
  bool checkVertical(const bitset<64> &locations) const {
    bitset<64> verticalBits = locations;
    // each 1-bit means there were 2 in a column vertically, except near the end
    // of a column
    verticalBits &= verticalBits >> 8;
    // if there was a 2-chain 2 columns over and a 2-chain here, then there's a
    // 4-chain
    verticalBits &= verticalBits >> 16;
    // vertical 4-chains don't exist beginning at the 6-th row
    // bits from 6-th row and up are shifted out of existence
    // https://en.cppreference.com/w/cpp/utility/bitset/operator_ltltgtgt
    // a right shift would shift the first 24 bits out of existence
    // verticalBits <<= (8 * 3);
    // any 1-bit means there's a vertical chain
    return verticalBits.any();
  }
  // check if the board is full (will be wrong if the last move is a winning
  // move)
  bool checkTie() const {
    // if there's a disk in every location
    return (getXLocations() | getOLocations()).all();
  }
  // checks whether the disk locations indicate a won position
  bool checkWin(const bitset<64> &locations) const {
    return checkDiagonal(locations) || checkHorizontal(locations) ||
           checkVertical(locations);
  }
  // returns the type of victory or INCOMPLETE
  GAME_STATE checkWin() const {
    if (checkWin(getXLocations()))
      return X_VICTORY;
    if (checkWin(getOLocations()))
      return O_VICTORY;
    return INCOMPLETE;
  }
  // returns the type of victory, a TIE, or INCOMPLETE if neither
  GAME_STATE getState() const {
    GAME_STATE winState = checkWin();
    if (winState == X_VICTORY || winState == O_VICTORY)
      return winState;
    if (checkTie())
      return TIE;
    return INCOMPLETE;
  }
  // get locations of X disks
  inline const bitset<64> &getXLocations() const { return diskLocations[0]; }
  // get locations of O disks
  inline const bitset<64> &getOLocations() const { return diskLocations[1]; }
  // get X or O locations depending on Disk
  bitset<64> &getLocations(Disk disk) {
    return diskLocations[(unsigned char)disk.type];
  }

  // same as getLocations() but const
  const bitset<64> &getLocations(Disk disk) const {
    return diskLocations[(unsigned char)disk.type];
  }

  // returns whether adding a disk to the column would be valid
  bool validMove(unsigned char col) {
    unsigned char row = columnHeights[col];
    return row != 8;
  }
  // adds a disk to the column, returns whether it was successful
  bool addDisk(Disk disk, unsigned char col) {
    unsigned char row = columnHeights[col];
    if (row == 8)
      return false;
    addDisk(getLocations(disk), row, col);
    ++columnHeights[col];
    return true;
  }
  // updates the location of a new disk
  inline void addDisk(bitset<64> &locations, unsigned char row,
                      unsigned char col) {
    locations.set(8 * row + col);
  }
  // removes the location of a disk
  inline void removeDisk(bitset<64> &locations, unsigned char row,
                         unsigned char col) {
    locations.reset(8 * row + col);
  }
  // removes the first disk in the column, assuming that it's the disk given
  void popDisk(Disk disk, unsigned char col) {
    unsigned char row = columnHeights[col] - 1;
    removeDisk(getLocations(disk), row, col);
    --columnHeights[col];
  }
  // get number of disks added
  unsigned char getDisksAdded() const {
    unsigned char added = 0;
    for (int i = 0; i < 8; ++i)
      added += columnHeights[i];
    return added;
  }

  bool hasDisk(const bitset<64> &locations, unsigned char row,
               unsigned char col) const {
    unsigned char position = 8 * row + col;
    return locations[position];
  }

  Disk getDisk(unsigned char row, unsigned char col) const {
    if (hasDisk(getXLocations(), row, col))
      return X;
    if (hasDisk(getOLocations(), row, col))
      return O;
    return EMPTY;
  }

  // returns a score based on number of adjacent 1-bits
  unsigned adjacencyScore(Disk disk, unsigned shift) const {
    unsigned score = 0;
    unsigned i = 1;
    bitset<64> locations = getLocations(disk);
    while (locations.any()) {
      locations &= locations >> shift;
      score += i * locations.count();
      ++i;
    }
    return score;
  }

  static constexpr bitset<64> horizontalAdjacencyMask() {
    return rowMask(1 << 7);
  }

  // score for horizontal adjacents that excludes cross-row adjacency
  // the specialization doesn't seem to be necessary
  unsigned horizontalAdjacencyScore(Disk disk) const {
    unsigned score = 0;
    unsigned i = 1;
    bitset<64> locations = getLocations(disk);
    while (locations.any()) {
      locations &= locations >> 1;
      locations &= horizontalAdjacencyMask();
      score += i * locations.count();
      ++i;
    }
    return score;
  }

  // returns a score based on number of adjacent 1-bits
  // for position scoring heuristic
  unsigned adjacencyScore(Disk disk) const {
    unsigned score = 0;
    // horizontal adjacents
    score += horizontalAdjacencyScore(disk); // adjacencyScore(disk, 1);
    // vertical adjacents
    score += adjacencyScore(disk, 8);
    // left diagonal adjacents
    score += adjacencyScore(disk, 7);
    // right diagonal adjacents
    score += adjacencyScore(disk, 9);
    return score;
  }

  operator Board<8, 8>() const {
    Board<8, 8> board;
    for (int col = 0; col < 8; ++col) {
      for (int row = 0; row < columnHeights[col]; ++row) {
        Disk disk = getDisk(row, col);
        board.addDisk(disk, col);
      }
    }
    return board;
  }
  operator string() {
    stringstream sout;
    Board<8, 8>(*this).display(sout);
    return sout.str();
  }

private:
  // 1 bit if disk is at position corresponding to bit
  // each bit is a column of a row, and every 8 bits is a new row
  // first in array is locations of X disks, second is for O disks
  bitset<64> diskLocations[2];
  // height of each column in disks
  unsigned char columnHeights[8] = {};

  friend class std::hash<BitBoard>;
};

#endif /* BITBOARD_H */
