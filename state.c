#include "state.h"
#include "lookups.h"

#include <stdlib.h>
#include <string.h>


// Updates if the given square is captured, as well as any connected
// squares in a captured region. Typically called after a branch is
// placed, to determine if the squares adjacent to the branch are now
// captured.
// Returns whether a capture took place.
bool State_updateCaptured(struct State *state, int square) {
    /* We can't do this right now, as this is used for undoing as well.
    if (state->captured[0] & (1llu << square)
            || state->captured[1] & (1llu << square)) {
        return false;
    }
    */

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

    while (size) {
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

    // TODO Add blocks to branches being built by opponent within
    // captured region

    // If we've gotten here, the searched squares are a captured region;
    // mark them all as captured by player
    for (int i = 0; i < NUM_SQUARES; i++) {
        if (crumbs[i]) {
            state->captured[player] |= (1llu << i);
        }
    }

    return true;

    nocapture:
    // Normally we could leave state alone if there are no captures.
    // However, we want to support undo, and thus we'll need to mark the
    // squares we visited as uncaptured (since they may have been
    // captured by a move being undone).
    // TODO It might be worth it to create a separate version of this
    // function, or by some other means skip this processing when not
    // undoing (especially since we'll never capture by undoing either).
    for (int i = 0; i < NUM_SQUARES; i++) {
        if (crumbs[i]) {
            for (enum Player p = 0; p < NUM_PLAYERS; p++) {
                state->captured[p] &= ~(1llu << i);
            }
        }
    }

    return false;
}


int State_largestNetworkSize(const struct State *state, enum Player player) {
    uint_fast64_t branches = state->branches[player];    

    uint_fast64_t networks[NUM_SQUARES] = {0};
    uint_fast64_t networksAdjacents[NUM_SQUARES] = {0};
    int count = 0;

    while (branches) {
        int branch = bitscan(branches);
        branches ^= (1llu << branch);

        if (count == 0) {
            networks[count] = (1llu << branch);
            networksAdjacents[count++] = EDGE_ADJACENT_EDGES[branch];
        } else {
            for (int i = 0; i < count; i++) {
                if ((1llu << branch) & networksAdjacents[i]) {
                    networks[i] |= (1llu << branch);
                    networksAdjacents[i] |= EDGE_ADJACENT_EDGES[branch];
                    goto nextbit;
                }
            }

            networks[count] =  (1llu << branch);
            networksAdjacents[count++] = EDGE_ADJACENT_EDGES[branch];
        }

        nextbit:
        continue;
    }

    int largestSize = 0;
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < i; j++) {
            if (networks[i] & networks[j]) {
                networks[i] |= networks[j];
            }
        }

        int networkSize = popcount(networks[i]);
        if (networkSize > largestSize) {
            largestSize = networkSize;
        }
    }

    return largestSize;
}


void State_randomStart(struct State *state) {
    // Most fields start off at 0
    memset(state, 0, sizeof(struct State));

    state->largestNetworkPlayer = PLAYER_NONE;

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
    for (enum Player player = 0; player < NUM_PLAYERS; player++) {
        state->score[player] = popcount(state->nodes[player]);
    }

    // Captured squares
    for (int i = 0; i < NUM_SQUARES; i++ ) {
        State_updateCaptured(state, i);
        for (enum Player player = 0; player < NUM_PLAYERS; player++) {
            if (state->captured[player] & (1llu << i)) {
                state->score[player]++;
                break;
            }
        }
    }

    // Largest network
    state->largestNetworkSize = 0;
    state->largestNetworkPlayer = PLAYER_NONE;
    for (enum Player player = 0; player < NUM_PLAYERS; player++) {
        int size = State_largestNetworkSize(state, player);
        if (size > state->largestNetworkSize) {
            state->largestNetworkSize = size;
            if (state->largestNetworkPlayer != PLAYER_NONE) {
                state->score[state->largestNetworkPlayer] -= LARGEST_NETWORK_SCORE;
            }
            state->score[player] += LARGEST_NETWORK_SCORE;
            state->largestNetworkPlayer = player;
        } else if (size == state->largestNetworkSize) {
            if (state->largestNetworkPlayer != PLAYER_NONE) {
                state->score[state->largestNetworkPlayer] -= LARGEST_NETWORK_SCORE;
            }
        }
    }

    /* Taking this out until it's implemented in State_act as well.
    // Derive phase
    state->phase = PLACE;
    if (popcount(state->nodes[PLAYER_1]) >= START_NODES){
        state->phase = PLAY;
    }
    */
}


struct Action *State_actions(const struct State *state) {
    const uint_fast8_t *resources = state->resources[state->turn];

    // TODO Verify this is the max combinations of trade
    struct Trade trades[41];
    int tradeCount = 0;

    trades[tradeCount++].active = false;

    for (enum Resource in1 = 0; in1 < NUM_RESOURCES; in1++ ) {
        for (enum Resource in2 = in1; in2 < NUM_RESOURCES; in2++ ) {
            for (enum Resource in3 = in2; in3 < NUM_RESOURCES; in3++ ) {
                int inResources[NUM_RESOURCES] = {0};
                inResources[in1]++;
                inResources[in2]++;
                inResources[in3]++;
                for (enum Resource r = 0; r < NUM_RESOURCES; r++) {
                    if (inResources[r] > resources[r]) {
                        goto next;
                    }
                }

                for (enum Resource out = 0; out < NUM_RESOURCES; out++ ) {
                    if (out == in1 || out == in2 || out == in3) {
                        continue;
                    }

                    trades[tradeCount].active = true;
                    trades[tradeCount].in[0] = in1;
                    trades[tradeCount].in[1] = in2;
                    trades[tradeCount].in[2] = in3;
                    trades[tradeCount].out = out;
                    tradeCount++;

                    printf("%d %d %d for %d\n", in1, in2, in3, out);
                }

                next:
                continue;
            }
        }
    }

    printf("%d\n", tradeCount);

    for (int i = 0; i < tradeCount; i++) {
        uint_fast8_t rs[NUM_RESOURCES];
        for (enum Resource r = 0; r < NUM_RESOURCES; r++) {
            rs[r] = resources[r];
        }

        if (trades[i].active) {
            for (int j = 0; j < TRADE_NUM; j++) {
                rs[trades[i].in[j]]--;
            }
            rs[trades[i].out]++;
        }

        int nodeBuilds = rs[GREEN] < rs[YELLOW] ?
            rs[GREEN] / 2 : rs[YELLOW] / 2;
        int branchBuilds = resources[BLUE] < resources[RED]
            ? rs[BLUE] : rs[RED];

        printf("%d nodes, %d branches\n", nodeBuilds, branchBuilds);

        // TODO we're going to need a way to determine where we can build
    }

}


void State_act(struct State *state, const struct Action *action) {
    #ifdef KELV_CHECKLEGAL
    // TODO Checks for legality will go here
    #endif

    enum Player turn = state->turn;
    
    // Process trade
    if (action->trade.active) {
        for (int i = 0; i < TRADE_NUM; i++) {
            state->resources[turn][action->trade.in[i]]--;
        }
        state->resources[turn][action->trade.out]++;
    }

    // Process branches
    state->branches[state->turn] |= action->branches;
    int branchCount = popcount(action->branches);

    // Pay resources for branches
    // TODO Don't pay resources in place phase
    state->resources[state->turn][RED] -= branchCount;
    state->resources[state->turn][BLUE] -= branchCount;

    // Check for new captured regions
    uint_fast64_t bits = action->branches;
    bool newCapture = false;
    while (bits) {
        int branch = bitscan(bits);
        bits ^= (1llu << branch);
        for (int i = 0; i < 2; i++) {
            int square = EDGE_ADJACENT_SQUARES[branch][i];
            if (square < 0) break;
            newCapture = State_updateCaptured(state, square) || newCapture;
        }
    }
    if (newCapture) {
        state->score[turn] = popcount(state->captured[turn]);
    }

    // Check for largest network changes
    if (action->branches) {
        int networkSize = State_largestNetworkSize(state, turn);
        // If we've got a new largest network size
        if (networkSize > state->largestNetworkSize) {
            state->largestNetworkSize = networkSize;
            // If the moving player didn't already have the largest ntwork
            if (state->largestNetworkPlayer != turn) {
                state->largestNetworkPlayer = turn;
                // If the other player previously had the largest
                // network, lower their score
                if (state->largestNetworkPlayer == !turn) {
                    state->score[!turn] -= LARGEST_NETWORK_SCORE;
                }
                // Raise current player's score
                state->score[turn] += LARGEST_NETWORK_SCORE;
            }
        // If current player has tied the largest network size, and did
        // not already have it
        } else if (networkSize == state->largestNetworkSize
                && turn != state->largestNetworkPlayer) {
            // If the other player had the largest network before, they
            // no longer have it
            if (state->largestNetworkPlayer == !turn) {
                state->score[!turn] -= LARGEST_NETWORK_SCORE;
            }
            state->largestNetworkPlayer = PLAYER_NONE;
        }
    }

    // Process nodes
    state->nodes[state->turn] |= action->nodes;
    int nodeCount = popcount(action->nodes);

    // Pay resources for nodes
    // TODO Don't pay resources in place phase
    state->resources[state->turn][YELLOW] -= nodeCount;
    state->resources[state->turn][GREEN] -= nodeCount;

    state->score[state->turn] += nodeCount;

    // TODO Check for exhaustion
    // TODO Update cache of resources/turn

    // Next player's turn
    state->turn = !turn;
}


void State_undo(struct State *state, const struct Action *action) {
    // Undo turn advance
    state->turn = !state->turn;
    enum Player turn = state->turn;

    // Undo trade
    if (action->trade.active) {
        for (int i = 0; i < TRADE_NUM; i++) {
            state->resources[turn][action->trade.in[i]]++;
        }
        state->resources[turn][action->trade.out]--;
    }

    // Undo branches
    // TODO Could this be subtract?
    state->branches[turn] &= ~action->branches;
    int branchCount = popcount(action->branches);

    // Refund resources for branches
    state->resources[turn][RED] += branchCount;
    state->resources[turn][BLUE] += branchCount;

    // Check if any squares are no longer captured
    uint_fast64_t bits = action->branches;
    while (bits) {
        int branch = bitscan(bits);
        bits ^= (1llu << branch);
        for (int i = 0; i < 2; i++) {
            int square = EDGE_ADJACENT_SQUARES[branch][i];
            if (square < 0) break;
            State_updateCaptured(state, square);
        }
    }
    // TODO Can/should we make this conditional?
    state->score[turn] = popcount(state->captured[turn]);

    // Check for largest network changes
    if (action->branches) {
        int size1 = State_largestNetworkSize(state, PLAYER_1);
        int size2 = State_largestNetworkSize(state, PLAYER_2);
        if (size1 > size2) {
            state->largestNetworkSize = size1;
            state->largestNetworkPlayer = PLAYER_1;
            state->score[PLAYER_1] += 2;
        } else if (size2 > size1) {
            state->largestNetworkSize = size2;
            state->largestNetworkPlayer = PLAYER_2;
            state->score[PLAYER_2] += 2;
        } else {
            state->largestNetworkSize = size1;
            state->largestNetworkPlayer = PLAYER_NONE;
        }
    }

    // Undo nodes
    state->nodes[state->turn] &= ~action->nodes;
    int nodeCount = popcount(action->nodes);

    // Refund resources for nodes
    state->resources[state->turn][YELLOW] += nodeCount;
    state->resources[state->turn][GREEN] += nodeCount;

    // Update score
    // TODO Can we do this without popcounting?
    state->score[state->turn] += popcount(state->nodes[turn]);
}
