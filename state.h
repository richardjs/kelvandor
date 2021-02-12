#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#define NUM_PLAYERS 2
#define NUM_RESOURCES 4
#define MAX_LIMIT 3
#define NUM_SQUARES (3*4 + 1)
#define NUM_CORNERS 24
#define NUM_EDGES 36
#define START_NODES 2
#define TRADE_NUM 3

enum Player {PLAYER_1=0, PLAYER_2};
enum Resource {RED=0, YELLOW, BLUE, GREEN};
enum Phase {PLACE, PLAY};


struct Square {
    enum Resource resource;
    uint_fast8_t limit;
};


struct State {
    // Unchanging (per game) information:
    struct Square squares[NUM_SQUARES];

    // Core information:
    uint_fast32_t nodes[2];
    uint_fast64_t branches[2];
    uint_fast8_t resources[NUM_PLAYERS][NUM_RESOURCES];
    enum Player turn;

    // Derived information:
    // - What squares each player has captured
    uint_fast8_t captured[NUM_PLAYERS];
    uint_fast8_t capturedCount[NUM_PLAYERS];

    uint_fast8_t score[NUM_PLAYERS];

    enum Phase phase;
};


struct Trade {
    enum Resource in[TRADE_NUM];
    enum Resource out;
};


struct Action {
    struct Trade *trade;
    uint_fast64_t branches;
    uint_fast32_t nodes;
};


// Start a game, with a standard random layout
void State_randomStart(struct State *state);

// Calculate the derived information from code information. Useful when
// loading a state or making changes outside of normal operations.
void State_derive(struct State *state);

void State_act(struct State *state, const struct Action *action);
void State_undo(struct State *state, const struct Action *action);

// Print state to stderr
void State_print(const struct State *state);

#endif
