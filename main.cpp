// Xingzhe Li, Daniel Roche, Jianqi Shi, Ching-Heng Hsiao
#include <fstream>
#include <queue>

#include "disk.h"
#include "board.h"
#include "timedAgent.h"

int main() {
  cout << "welcome to four-in-row game!" << endl;
  Board board;
  queue<int> inputs;

  Disk currentDisk = X;
  Disk diskSelection;
  cout << "Please choose either X or O to play as" << endl;
  cin >> diskSelection;
  cout << "You've selected to play as " << diskSelection << ". Begin!" << endl;

  TimedAgent opponent;
  while (cin) {
    int columnChoice = 8;
    // if it's the player's turn
    if (currentDisk == diskSelection) {
      do {
        cout << "The current disk is: " << currentDisk << '\n';
        cout << "Enter a column from 1 to 8: ";
        cin >> columnChoice;
        columnChoice--;
        // output error message if it isn't a valid move
      } while (!board.validMove(columnChoice) && cin &&
               cout << "Invalid column. Please try again\n");
      inputs.push(columnChoice + 1);
    }
    // opponent's turn
    else {
      BitBoard bitboard(board);
      opponent.setBoardP(&bitboard);
      // this is only necessary for self-play
      opponent.setPlayer(currentDisk);
      columnChoice = opponent.chooseColumn();
    }
    board.addDisk(currentDisk, columnChoice);

    board.display();
    cout << "Chosen column: " << (columnChoice + 1) << '\n';
    GAME_STATE state = board.getState();

    cout << '\n';

    if (state == X_VICTORY) {
      cout << "Player 1 (X) wins !\n";
      if (diskSelection == X)
        cout << "You win!\n";
      else if (diskSelection == O)
        cout << "You lose!\n";
      break;
    } else if (state == O_VICTORY) {
      cout << "Player 2 (O) wins !\n";
      if (diskSelection == O)
        cout << "You win!\n";
      else if (diskSelection == X)
        cout << "You lose!\n";
      break;
    } else if (state == TIE) {
      cout << "The game is a tie !\n";
      break;
    }
    currentDisk.alternate();
  }

  cout << "Average time taken: " << opponent.averageTime() << " ns\n";
  ofstream inputLog("inputs", ios_base::trunc);
  while (inputs.size()) {
    inputLog << inputs.front() << '\n';
    inputs.pop();
  }

  return 0;
}
