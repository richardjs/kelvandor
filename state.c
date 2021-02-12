#include "lookups.h"
#include "state.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define popcount(x) __builtin_popcount(x)
#define bitscan(x) __builtin_ctzl(x)


// Updates if the given square is captured, as well as any connected
// squares in a captured region. Typically called after a branch is
// placed, to determine if the squares adjacent to the branch are now
// captured.
void State_updateCaptured(struct State *state, int square) {
    // Search mechanics
    int stack[NUM_SQUARES];
    int size = 0;
    stack[size++] = square;

    bool crumbs[NUM_SQUARES] = {false};

    // Tracks what player might own the searched region. -1 means no
    // player determined yet. Updated when we find a bordering branch.
    // If we find bordering branches owned by both players, no one
    // captures the region.
    int player = -1;

    while(size) {
        int sq = stack[--size];  
        crumbs[sq] = true;
        
        // Check each direction for a branch
        for (enum Direction dir = NORTH; dir < 4; dir++) {
            uint_fast64_t branch = (1llu << SQUARE_ADJACENT_BRANCHES[sq][dir]);

            // If P1 has a branch in this direction...
            if (state->branches[PLAYER_1] & branch) {
                // If player is still -1, now we're checking if P1 owns
                // the region
                if (player < 0) {
                    player = PLAYER_1;
                // Else if player is already P2 (meaning we've already
                // found a P2 branch bordering the region), no one owns
                // the region
                } else if (player == PLAYER_2) {
                    // Use goto to break out of the nested loop and skip
                    // marking searched squares as captured
                    goto nocapture;
                }
            // This mirrors the above P1 branch
            } else if (state->branches[PLAYER_2] & branch) {
                if (player < 0) {
                    player = PLAYER_2;
                } else if (player == PLAYER_1) {
                    goto nocapture;
                }
            // If there is not a branch in this direction, see if
            // there's another square that may be part of a larger
            // region
            // (This is the branch that continues the search)
            } else if (SQUARE_ADJACENT_SQUARES[sq][dir] > 0) {
                int adjacent = SQUARE_ADJACENT_SQUARES[sq][dir];
                if (!crumbs[adjacent]) {
                    stack[size++] = adjacent;
                }
            // If there's no branches in this direction and not another
            // square, we must be on the edge of the board (without a
            // bounding branch), and thus this region is not captured
            } else {
                goto nocapture;
            }
        }
    }

    // TODO We need to check for nodes within a region as well--it
    // can't be captured if there's the other player's nodes in it

    // If we've gotten here, the searched squares are a captured region;
    // mark them all as captured by player
    for (int i = 0; i < NUM_SQUARES; i++) {
        if (crumbs[i]) {
            state->captured[player] |= (1llu << i);
        }
    }

    nocapture:
    return;
}


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

    enum Player turn = state->turn;
    
    // Process trade
    if (action->trade != NULL) {
        for (int i = 0; i < TRADE_NUM; i++) {
            state->resources[turn][action->trade->in[i]]--;
        }
        state->resources[turn][action->trade->out]++;
    }

    // Process branches
    state->branches[state->turn] |= action->branches;
    int branchCount = popcount(action->branches);

    // Pay resources for branches
    // TODO Don't pay resources in place phase
    state->resources[state->turn][RED] -= branchCount;
    state->resources[state->turn][BLUE] -= branchCount;

    uint_fast64_t bits = action->branches;
    while(bits) {
        int branch = bitscan(bits);
        bits ^= (1llu << branch);
        for (int i = 0; i < 2; i++) {
            int square = EDGE_ADJACENT_SQUARES[branch][i];
            if (square < 0) break;
            State_updateCaptured(state, square);
        }
    }
    // TODO Score processing and whatever else after captures are updated?
    // TODO Check for largest network

    // Process nodes
    state->nodes[state->turn] |= action->nodes;
    int nodeCount = popcount(action->nodes);

    // Pay resources for nodes
    // TODO Don't pay resources in place phase
    state->resources[state->turn][YELLOW] -= nodeCount;
    state->resources[state->turn][GREEN] -= nodeCount;

    // Update score
    state->score[state->turn] += nodeCount;

    // TODO Check for exhaustion
    // TODO Update cache of resources/turn

    // Next player's turn
    state->turn = !turn;
}


void State_undo(struct State *state, const struct Action *action) {
    enum Player turn = state->turn;

    // Undo trade
    if (action->trade != NULL) {
        for (int i = 0; i < TRADE_NUM; i++) {
            state->resources[turn][action->trade->in[i]]++;
        }
        state->resources[turn][action->trade->out]--;
    }

    // Undo branches
    // TODO Could this be subtract?
    state->branches[state->turn] &= ~action->branches;
    int branchCount = popcount(action->branches);

    // Refund resources for branches
    state->resources[state->turn][RED] += branchCount;
    state->resources[state->turn][BLUE] += branchCount;

    // Undo nodes
    state->nodes[state->turn] &= ~action->nodes;
    int nodeCount = popcount(action->nodes);

    // Refund resources for nodes
    state->resources[state->turn][YELLOW] += nodeCount;
    state->resources[state->turn][GREEN] += nodeCount;

    // Update score
    state->score[state->turn] -= nodeCount;

    // Previous player's turn
    state->turn = !turn;
}
