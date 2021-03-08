#ifndef MCTS_H
#define MCTS_H


#include "state.h"


#define ITERATIONS 10000
#define MAX_SIM_DEPTH 1000


int mcts(const struct State *state);


#endif
