#ifndef AGENT_H
#define AGENT_H

#include <vector>
#include "disk.h"
#include "bitBoard.h"
using namespace std;

// class for preparing to make a move using the player's disk and the board
class Agent {
public:
  Agent() {}
  Agent(BitBoard *boardP, Disk player) {
    this->boardP = boardP;
    this->player = player;
  }

  // returns whether the state is a victory for the current player
  bool isVictory(GAME_STATE state) {
    switch (player.type) {
    case X:
      return state == X_VICTORY;
    case O:
      return state == O_VICTORY;
    default:
      return false;
    }
  }

  // returns board where the player added a disk to the column
  /*Board<> nextBoard(int column)
  {
    Board<> future = board;
    // we are using addDisk() here just for strategizing,
    // this is a reason why we shouldn't have output in addDisk()
    // and instead separate it into another function/class
    future.addDisk(player, column);
    return future;
  }*/

  // returns columns that would cause the player to win immediately
  vector<int> currentWinningMoves() {
    vector<int> winningMoves;
    for (int col = 0; col < 8; ++col) {
      if (boardP->addDisk(player, col)) {
        GAME_STATE futureState = boardP->checkWin();
        if (isVictory(futureState)) {
          winningMoves.push_back(col);
        }
        boardP->popDisk(player, col);
      }
    }
    return winningMoves;
  }

  // returns agent of a board where the current player added a disk to the
  // column
  Agent nextAgent(int column) {
    Disk nextPlayer = player.counterpart();
    boardP->addDisk(player, column);
    return Agent(boardP, nextPlayer);
  }

  // default search depth
  static constexpr int DEFAULT_DEPTH = 10;
  // default minimum score
  static constexpr int DEFAULT_ALPHA = -(1 << 30);
  // default maximum score
  static constexpr int DEFAULT_BETA = 1 << 30;

  // maps an index from 0 to 7 into a row alternating away from center
  // i.e. { 0, 1, ..., 7 } to { 4, 3, 5, 2, 6, 1, 7, 0 }
  static int alternatingColumn(int index) {
    return 4 + (1 - 2 * (index % 2)) * (index + 1) / 2;
  }

  // returns a score for the current position before our move
  // based on opponent's best responses
  // alpha tracks the worst possible score we can have based on our best moves
  // and opponent's best moves analyzed so far
  // beta is the best possible score we can have based on the opponent's best
  // known move so far
  // requiredDepth is how many moves into the future after our move we need to
  // consider
  int evaluatePosition(int requiredDepth = DEFAULT_DEPTH,
                       int alpha = DEFAULT_ALPHA, int beta = DEFAULT_BETA) {
    // return a high score if the player can win immediately
    // return a low score if the player will lose next move
    // based heavily on Wikipedia's pseudocode
    // https://en.wikipedia.org/wiki/Minimax
    // https://en.wikipedia.org/wiki/Negamax
    // https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning
    // our score will be determined by the move that gives the lowest score to
    // the next player
    // use recursion to find the scores of the future positions
    // if depth == maxDepth, do not evaluate any future positions

    if (currentWinningMoves().size() > 0) {
      return DEFAULT_BETA;
    }

    // score for not evaluating any future positions after checking for
    // victory
    if (requiredDepth == 0) {
      BitBoard &board = *boardP;
      // the score increases based on number of adjacent disks and ways they're
      // adjacent
      return board.adjacencyScore(player) -
             board.adjacencyScore(player.counterpart());
    }

    int score = DEFAULT_ALPHA;

    // if the opponent's worst possible score by our move is worse than our
    // worst possible score, we'll assume the opponent won't let us get to this
    // position
    for (int i = 0; beta > alpha && i < 8; ++i) {
      // start searching near the center first
      // alternate between left and right of center
      int col = alternatingColumn(i);
      if (boardP->validMove(col)) {
        int scoreAfterMove =
            evaluatePositionAfterMove(col, alpha, beta, requiredDepth);
        if (scoreAfterMove > score)
          score = scoreAfterMove;
      }
    }

    return score;
  }

  // evaluates the position after a hypothetical move from the player is made
  // order of parameters is for default parameters used in chooseColumn()
  int evaluatePositionAfterMove(int col, int &alpha, int beta = DEFAULT_BETA,
                                const int &requiredDepth = DEFAULT_DEPTH) {
    Agent opponent = nextAgent(col);
    // if opponent's score is better than their score for a different move of
    // ours, we will not make that move, so their beta will be their worst
    // possible score known so far
    int opponentScore =
        opponent.evaluatePosition(requiredDepth - 1, -beta, 1 - alpha);
    // if we prefer winning quickly, we can reduce the penalty
    // of the next player's good move by 1, so the farther their win is, the
    // less bad our score
    int score = 1 - opponentScore;
    if (score > alpha)
      alpha = score;

    boardP->popDisk(player, col);
    return score;
  }

  // chooses a column to add to
  int chooseColumn() {
    for (int i = 0; i < 2; ++i, player.alternate()) {
      vector<int> winningMoves = currentWinningMoves();
      // return the first winning move if there is one
      // block the opponent's winning move second iteration
      if (winningMoves.size())
        return winningMoves.front();
    }

    unsigned char bestCol = 4;
    int alpha = DEFAULT_ALPHA;
    int scores[8];
    for (unsigned char i = 0; i < 8; ++i) {
      int col = alternatingColumn(i);
      // to avoid invalid moves
      // an invalid move has a worse score than all valid ones
      scores[col] = DEFAULT_ALPHA - 1;

      if (boardP->validMove(col)) {
        int score = scores[col] = evaluatePositionAfterMove(col, alpha);
        // choose move with best score
        if (score > scores[bestCol]) {
          bestCol = col;
        }
      }
    }

    cout << "scores: ";
    for (int i : scores)
      cout << i << ' ';
    cout << '\n';

    return bestCol;
  }

  void setBoardP(BitBoard *boardP) { this->boardP = boardP; }
  void setPlayer(Disk player) { this->player = player; }

private:
  BitBoard *boardP;
  Disk player;
};

#endif /* AGENT_H */
