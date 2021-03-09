#ifndef MCTS_H
#define MCTS_H


#include "state.h"


#define ITERATIONS 100000
#define MAX_SIM_DEPTH 500


int mcts(const struct State *state);


#endif
