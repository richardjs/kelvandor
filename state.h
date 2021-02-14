#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include <stdint.h>

#define NUM_PLAYERS 2
#define NUM_RESOURCES 4
#define MAX_LIMIT 3
#define NUM_SQUARES (3*4 + 1)
#define NUM_CORNERS 24
#define NUM_EDGES 36
#define START_NODES 2
#define TRADE_NUM 3

enum Player {PLAYER_1=0, PLAYER_2, PLAYER_NONE};
enum Resource {RED=0, YELLOW, BLUE, GREEN};
enum Phase {PLACE, PLAY};


struct Square {
    enum Resource resource;
    uint_fast8_t limit;
};


struct State {
    /* Static information */
    struct Square squares[NUM_SQUARES];

    /* Core information */

    uint_fast32_t nodes[NUM_PLAYERS];
    uint_fast64_t branches[NUM_PLAYERS];
    uint_fast8_t resources[NUM_PLAYERS][NUM_RESOURCES];
    enum Player turn;

    /* Derived information */

    // Each player's resources-per-turn income
    uint_fast8_t income[NUM_PLAYERS][NUM_RESOURCES];

    // What squares each player has captured
    uint_fast8_t captured[NUM_PLAYERS];

    uint_fast8_t largestNetworkSize;
    enum Player largestNetworkPlayer;

    uint_fast8_t score[NUM_PLAYERS];

    enum Phase phase;
};


struct Trade {
    bool active;
    enum Resource in[TRADE_NUM];
    enum Resource out;
};


struct Action {
    struct Trade trade;
    uint_fast64_t branches;
    uint_fast32_t nodes;
};


/* Initializers */

// Start a game, with a standard random layout
void State_randomStart(struct State *state);


/* Core operations */

void State_act(struct State *state, const struct Action *action);
void State_undo(struct State *state, const struct Action *action);


/* Interface */

// Print state to stderr
void State_print(const struct State *state);
void State_printDetail(const struct State *state);


/* Internal */

// Calculate the derived information from code information. Useful when
// loading a state or making changes outside of normal operations.
void State_derive(struct State *state);

void State_updateCaptured(struct State *state, int square);


#endif
