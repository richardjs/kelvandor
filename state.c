#include "state.h"

#include <stdlib.h>
#include <string.h>


void State_randomStart(struct State *state) {
    // Most fields start off at 0
    memset(state, 0, sizeof(struct State));
    
    // Randomize the starting squares by interating through all squares
    // and picking a random location for each. The vacant square will
    // be at the last location
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
