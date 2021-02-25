#ifndef STATE_H
#define STATE_H


#include <stdbool.h>
#include <stdint.h>


#define popcount(x) __builtin_popcount(x)
#define bitscan(x) __builtin_ctzl(x)

#define NUM_PLAYERS 2
#define NUM_RESOURCES 4
#define MAX_LIMIT 3
#define NUM_SQUARES (3*4 + 1)
#define NUM_CORNERS 24
#define NUM_EDGES 36
#define START_NODES 2
#define LARGEST_NETWORK_SCORE 2
#define TRADE_IN_NUM 3
#define MAX_TRADE_COMBINATIONS 40
#define MAX_ACTIONS (MAX_TRADE_COMBINATIONS + NUM_EDGES + NUM_CORNERS + 1)
#define BRANCH_COST 1
#define NODE_COST 2


enum Player {PLAYER_1=0, PLAYER_2, PLAYER_NONE};
enum Resource {RED=0, YELLOW, BLUE, GREEN};
enum ActionType {START_PLACE, TRADE, BRANCH, NODE, END};


struct Square {
    enum Resource resource;
    uint_fast8_t limit;
    uint_fast8_t remaining;
    enum Player captor;
};


struct Action {
    enum ActionType type;
    
    // Used with START_PLACE, BRANCH, and NODE types
    // With START_PLACE, location indicates the placed node, and bits
    // 6-7 indicating the direction of the accompanying edge
    //uint_fast8_t location;
    uint_fast8_t data;
};


struct State {
    /* Core information */
    struct Square squares[NUM_SQUARES];
    uint_fast32_t nodes[NUM_PLAYERS];
    uint_fast64_t branches[NUM_PLAYERS];
    uint_fast8_t resources[NUM_PLAYERS][NUM_RESOURCES];
    enum Player turn;
    bool tradeDone;

    /* Derived information */

    // What squares each player has captured
    uint_fast8_t captured[NUM_PLAYERS];

    uint_fast8_t largestNetworkSize;
    enum Player largestNetworkPlayer;

    uint_fast8_t score[NUM_PLAYERS];

    struct Action actions[MAX_ACTIONS];
    uint_fast8_t actionCount;
};


/* Initializers */

// Start a game, with a standard random layout
void State_randomStart(struct State *state);


/* Core operations */

//struct Action *State_actions(const struct State *state);
void State_act(struct State *state, const struct Action *action);
void State_undo(struct State *state, const struct Action *action);


/* Interface */

// Print state to stderr
void State_print(const struct State *state);
void State_printDetail(const struct State *state);


#endif
