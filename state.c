#include "state.h"

#include <stdlib.h>
#include <string.h>


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
    // Derive nodeCount
    memset(state->nodeCount, 0, sizeof(state->nodeCount)*NUM_PLAYERS);
    for (int i = 0; i < NUM_CORNERS; i++) {
        uint_fast32_t bit = 1lu << i;
        for(enum Player player = 0; player < NUM_PLAYERS; player++) {
            if (bit & state->nodes[player]) {
                state->nodeCount[player] += 1;
            }
        }
    }

    // Derive scores
    // TODO Account for captured squares
    // TODO Account for largest network
    for (enum Player player = 0; player < NUM_PLAYERS; player++) {
        state->scores[player] = state->nodeCount[player];
    }

    // Derive phase
    state->phase = PLACE;
    if (state->nodeCount[PLAYER_1] >= 2){
        state->phase = PLAY;
    }
}
