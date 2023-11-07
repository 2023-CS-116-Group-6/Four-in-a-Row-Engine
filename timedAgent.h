#ifndef TIMEDAGENT_H
#define TIMEDAGENT_H

#include <ctime>
#include "agent.h"
using namespace std;

class TimedAgent : public Agent {
public:
  // inherit constructors
  using Agent::Agent;

  // print time information, then return move
  int chooseColumn() {
    time_t current = time(0);
    clock_t start = clock();

    cout << "started planning move at: " << ctime(&current) << '\n';

    int col = Agent::chooseColumn();
    ++moves;

    clock_t end = clock();
    current = time(0);
    cout << "finished planning move at: " << ctime(&current) << '\n';

    clock_t taken = end - start;
    total_time += taken;

    cout << "took " << ((double)taken) / CLOCKS_PER_SEC << " seconds\n";
    return col;
  }
  // return average time taken per move in ns
  double averageTime() { return (total_time * 1e9) / CLOCKS_PER_SEC / moves; }

private:
  int moves = 0;
  clock_t total_time = 0;
};


#endif /* TIMEDAGENT_H */
