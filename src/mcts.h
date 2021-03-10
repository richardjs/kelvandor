#ifndef MCTS_H
#define MCTS_H


#include "state.h"


#define ITERATIONS 100000
#define MAX_SIM_DEPTH 500
#define UCTC 2
#define MULTIACTION true


struct Node {
    bool expanded;
    unsigned int visits;
    float value;

    struct Node **children;
    uint_fast8_t childrenCount;

    uint8_t depth;
};


void Node_init(struct Node *node, uint8_t depth);


// root should be NULL for fresh calls; it is there to reuse existing
// search information from previous calls
void mcts(const struct State *state, struct Node *root);


#endif
