#include "state.h"

#include <stdlib.h>
#include <string.h>


#define popcount(x) __builtin_popcount(x)


void State_randomStart(struct State *state) {
    // Most fields start off at 0
    memset(state, 0, sizeof(struct State));
    
    // Randomize the starting squares by interating through all squares
    // and picking a random location for each. The vacant square will
    // be at the last location.
    for (enum Resource resource = 0; resource < NUM_RESOURCES; resource++) {
        for (int limit = 1; limit <= MAX_LIMIT; limit ++){
            // Find a place that does not already have a square. All
            // squares but the vacant have a limit > 0.
            int try_place;
            do {
                try_place = rand() % NUM_SQUARES;
            } while (state->squares[try_place].limit > 0);

            state->squares[try_place].resource = resource;
            state->squares[try_place].limit = limit;
        }
    }
}


void State_derive(struct State *state) {
    // Derive scores
    // TODO Account for captured squares
    // TODO Account for largest network
    for (enum Player player = 0; player < NUM_PLAYERS; player++) {
        state->score[player] = popcount(state->nodes[player]);
    }

    // Derive phase
    state->phase = PLACE;
    if (popcount(state->nodes[PLAYER_1]) >= START_NODES){
        state->phase = PLAY;
    }
}


void State_act(struct State *state, const struct Action *action) {
    #ifdef KELV_CHECKLEGAL
    // TODO Checks for legality will go here
    #endif
    
    // Process trade
    enum Player turn = state->turn;
    if (action->trade != NULL) {
        for(int i = 0; i < TRADE_NUM; i++) {
            state->resources[turn][action->trade->in[i]]--;
        }
        state->resources[turn][action->trade->out]++;
    }

    // Add nodes
    state->nodes[state->turn] |= action->nodes;
    state->score[state->turn] = popcount(state->nodes[state->turn]);
    // TODO Check for exhaustion
    // TODO Update cache of resources/turn

    // Add branches
    state->branches[state->turn] |= action->branches;
    // TODO Check for captures
    // TODO Check for largest network

    // Next player's turn
    state->turn = !turn;
}
