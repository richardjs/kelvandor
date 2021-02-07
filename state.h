#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#define NUM_PLAYERS 2
#define NUM_RESOURCES 4
#define MAX_LIMIT 3
#define NUM_SQUARES (3*4 + 1)
#define NUM_CORNERS 24
#define NUM_EDGES 36

typedef uint_fast32_t nodebits;
typedef uint_fast64_t branchbits;

enum Player {PLAYER_1=0, PLAYER_2};
enum Resource {RED=0, YELLOW, BLUE, GREEN};


struct Square {
    enum Resource resource;
    uint_fast8_t limit;
};


struct State {
    // Unchanging information
    struct Square squares[NUM_SQUARES];

    // Core information
    nodebits nodes[2];
    branchbits branches[2];

    uint_fast8_t playerResources[NUM_PLAYERS][NUM_RESOURCES];

    enum Player turn;

    // Derived information
    uint_fast8_t playerScores[NUM_PLAYERS];   
};

void State_randomStart(struct State *state);
void State_print(const struct State *state);

#endif
