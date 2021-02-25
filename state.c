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
            state->squares[i].captor = player;
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


void State_deriveStartActions(struct State *state) {
    state->actionCount = 0;

    uint_fast32_t openEdges = ~(state->branches[PLAYER_1] | state->branches[PLAYER_2]);
    openEdges &= (1llu << NUM_EDGES) - 1;
    uint_fast32_t openCorners = ~(state->nodes[PLAYER_1] | state->nodes[PLAYER_2]);
    openCorners &= (1llu << NUM_CORNERS) - 1;
    while(openCorners) {
        int bit = bitscan(openCorners);
        openCorners ^= (1llu << bit);

        for (enum Direction dir = 0; dir < 4; dir++) {
            if (CORNER_ADJACENT_EDGES[bit][dir] < 0) {
                continue;
            }
            if (openEdges & (1llu << CORNER_ADJACENT_EDGES[bit][dir])) {
                state->actions[state->actionCount].type = START_PLACE;
                state->actions[state->actionCount].data = bit;
                state->actions[state->actionCount].data |= (dir << 6);
                state->actionCount++;
            }
        }
    }

    // TODO only do this with test flag
    memset(&state->actions[state->actionCount], 0,
        sizeof(struct Action) * (MAX_ACTIONS - state->actionCount));
}


void State_deriveActions(struct State *state) {
    if (popcount(state->nodes[PLAYER_1]) < 2) {
        State_deriveStartActions(state);
        return;
    }

    state->actionCount = 0;

    if (!state->tradeDone) {
        for (enum Resource in1 = 0; in1 < NUM_RESOURCES; in1++ ) {
            for (enum Resource in2 = in1; in2 < NUM_RESOURCES; in2++ ) {
                for (enum Resource in3 = in2; in3 < NUM_RESOURCES; in3++ ) {
                    int inResources[NUM_RESOURCES] = {0};
                    inResources[in1]++;
                    inResources[in2]++;
                    inResources[in3]++;
                    for (enum Resource r = 0; r < NUM_RESOURCES; r++) {
                        if (inResources[r] > state->resources[state->turn][r]) {
                            goto next;
                        }
                    }

                    for (enum Resource out = 0; out < NUM_RESOURCES; out++ ) {
                        if (out == in1 || out == in2 || out == in3) {
                            continue;
                        }

                        state->actions[state->actionCount].type = TRADE;
                        state->actions[state->actionCount].data = in1 << 0;
                        state->actions[state->actionCount].data |= in2 << 2;
                        state->actions[state->actionCount].data |= in3 << 4;
                        state->actions[state->actionCount].data |= out << 6;
                        state->actionCount++;
                    }

                    next:
                    continue;
                }
            }
        }
    }

    uint_fast32_t adjacentCorners = 0;
    uint_fast64_t adjacentEdges = 0;
    uint_fast64_t bits = state->branches[state->turn];
    while (bits) {
        int bit = bitscan(bits);
        bits ^= (1llu << bit);
 
        adjacentCorners |= EDGE_ADJACENT_CORNERS[bit];
        adjacentEdges |= EDGE_ADJACENT_EDGES[bit];
    }
    uint_fast64_t openEdges = ~state->branches[PLAYER_1] & ~state->branches[PLAYER_2];
    openEdges &= (1llu << NUM_EDGES) - 1;
    adjacentEdges &= openEdges;
    uint_fast32_t openCorners = ~state->nodes[PLAYER_1] & ~state->nodes[PLAYER_2];
    openCorners &= (1llu << NUM_CORNERS) - 1;
    adjacentCorners &= openCorners;

    if (state->resources[state->turn][RED >= BRANCH_COST]
            && state->resources[state->turn][BLUE] >= BRANCH_COST) {
        bits = openEdges;
        while (bits) {
            int bit = bitscan(bits);
            bits ^= (1llu << bit);

            state->actions[state->actionCount].type = BRANCH;
            state->actions[state->actionCount].data = bit;
            state->actionCount++;
        }
    }

    if (state->resources[state->turn][YELLOW >= NODE_COST]
           && state->resources[state->turn][GREEN] >= NODE_COST) {
        bits = openCorners;
        while (bits) {
            int bit = bitscan(bits);
            bits ^= (1llu << bit);

            state->actions[state->actionCount].type = NODE;
            state->actions[state->actionCount].data = bit;
            state->actionCount++;
        }
    }

    state->actions[state->actionCount].type = END;

    // TODO only do this with test flag
    state->actions[state->actionCount].data = 0;

    state->actionCount++;

    // TODO only do this with test flag
    memset(&state->actions[state->actionCount], 0,
        sizeof(struct Action) * (MAX_ACTIONS - state->actionCount));
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

    State_deriveActions(state);
}


void State_collectResources(struct State *state, int sign) {
    uint_fast32_t nodeBits = state->nodes[state->turn]; 
    while (nodeBits) {
        int bit = bitscan(nodeBits);
        nodeBits ^= (1llu << bit);

        for (int i = 0; i < 4; i++) {
            int sq = CORNER_ADJACENT_SQUARES[bit][i];
            if (sq < 0) {
                continue;
            }
            if (state->squares[sq].remaining <= 0) {
                continue;
            }
            state->resources[state->turn][state->squares[sq].resource] += sign;
        }
    }
}


void State_act(struct State *state, const struct Action *action) {
    switch (action->type) {
        case START_PLACE: {
            int node = action->data & 0b11111;
            int dir = action->data >> 6;
            int branch = CORNER_ADJACENT_EDGES[node][dir];
            state->nodes[state->turn] |= (1llu << node);
            state->branches[state->turn] |= (1llu << branch);

            if (state->turn == PLAYER_1) {
                state->turn = PLAYER_2;
                if (popcount(state->nodes[PLAYER_2]) == 2) {
                    State_collectResources(state, 1);
                }
            } else if (popcount(state->nodes[PLAYER_2]) == START_NODES) {
                state->turn = PLAYER_1;
            }

            break;
        }

        case TRADE: {
            state->resources[state->turn][(action->data >> 0)  & 0b11]--;
            state->resources[state->turn][(action->data >> 2)  & 0b11]--;
            state->resources[state->turn][(action->data >> 4)  & 0b11]--;
            state->resources[state->turn][(action->data >> 6)  & 0b11]++;
            state->tradeDone = true;

            break;
        }

        case BRANCH: {
            state->resources[state->turn][RED]--;
            state->resources[state->turn][BLUE]--;
            state->branches[state->turn] |= (1llu << action->data);

            for (int i = 0; i < 2; i++) {
                int square = EDGE_ADJACENT_SQUARES[action->data][i];
                if (square < 0) break;
                State_updateCaptured(state, square);
            }
            // TODO update score if we've got a capture

            break;
        }

        case NODE: {
            state->resources[state->turn][YELLOW] -= 2;
            state->resources[state->turn][GREEN] -= 2;
            state->nodes[state->turn] |= (1llu << action->data);

            for (int i = 0; CORNER_ADJACENT_SQUARES[action->data][i] >= 0; i++) {
                state->squares[CORNER_ADJACENT_SQUARES[action->data][i]].remaining--;
            }
            // TODO update score

            break;
        }

        case END: {
            // TODO check for longest network here?

            state->turn = !state->turn;
            State_collectResources(state, 1);

            break;
        }
    }

    State_deriveActions(state);
}


void State_undo(struct State *state, const struct Action *action) {
    switch (action->type) {
        case START_PLACE: {
            if (state->turn == PLAYER_1) {
                state->turn = PLAYER_2;
            } else if (popcount(state->nodes[PLAYER_2]) != 1) {
                state->turn = PLAYER_1;
                for (enum Resource res = 0; res < NUM_RESOURCES; res++) {
                    state->resources[PLAYER_2][res] = 0;
                }
            }

            int node = action->data & 0b11111;
            int dir = action->data >> 6;
            int branch = CORNER_ADJACENT_EDGES[node][dir];
            state->nodes[state->turn] ^= (1llu << node);
            state->branches[state->turn] ^= (1llu << branch);

            break;
        }

        case TRADE: {
            state->resources[state->turn][(action->data >> 0)  & 0b11]++;
            state->resources[state->turn][(action->data >> 2)  & 0b11]++;
            state->resources[state->turn][(action->data >> 4)  & 0b11]++;
            state->resources[state->turn][(action->data >> 6)  & 0b11]--;
            state->tradeDone = false;

            break;
        }

        case BRANCH: {
            state->resources[state->turn][RED]++;
            state->resources[state->turn][BLUE]++;
            state->branches[state->turn] ^= (1llu << action->data);

            for (int i = 0; i < 2; i++) {
                int square = EDGE_ADJACENT_SQUARES[action->data][i];
                if (square < 0) break;
                State_updateCaptured(state, square);
            }
            break;
        }

        case NODE: {
            state->resources[state->turn][YELLOW] += 2;
            state->resources[state->turn][GREEN] += 2;
            state->nodes[state->turn] ^= (1llu << action->data);

            for (int i = 0; CORNER_ADJACENT_SQUARES[action->data][i] >= 0; i++) {
                state->squares[CORNER_ADJACENT_SQUARES[action->data][i]].remaining++;
            }
            // TODO update score

            break;
        }

        case END: {
            State_collectResources(state, -1);
            // TODO check for longest network here?
            state->turn = !state->turn;

            break;
        }
    }

    State_deriveActions(state);
}

void State_randomStart(struct State *state) { // Most fields start off at 0
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
            } while (state->squares[try_place].remaining > 0);

            state->squares[try_place].resource = resource;
            state->squares[try_place].limit = limit;
            state->squares[try_place].remaining = limit;
            state->squares[try_place].captor = PLAYER_NONE;
        }
    }

    State_derive(state);
}


//bool State_equal(const struct State *state, const struct State *other) {
//    for (int i = 0; i < NUM_SQUARES; i++) {
//        if (state->squares[i].resource != other->squares[i].resource) return false;
//        if (state->squares[i].remaining != other->squares[i].remaining) return false;
//        if (state->squares[i].captor != other->squares[i].captor) return false;
//    }
//
//    for (enum Player player = 0; player < NUM_PLAYERS; player++) {
//        if (state->nodes[player] != other->nodes[player]) return false;
//        if (state->branches[player] != other->branches[player]) return false;
//    }
//}


// void State_act(struct State *state, const struct Action *action) {
//     #ifdef KELV_CHECKLEGAL
//     // TODO Checks for legality will go here
//     #endif
// 
//     enum Player turn = state->turn;
//     
//     // Process trade
//     if (action->trade.active) {
//         for (int i = 0; i < TRADE_NUM; i++) {
//             state->resources[turn][action->trade.in[i]]--;
//         }
//         state->resources[turn][action->trade.out]++;
//     }
// 
//     // Process branches
//     state->branches[state->turn] |= action->branches;
//     int branchCount = popcount(action->branches);
// 
//     // Pay resources for branches
//     // TODO Don't pay resources in place phase
//     state->resources[state->turn][RED] -= branchCount;
//     state->resources[state->turn][BLUE] -= branchCount;
// 
//     // Check for new captured regions
//     uint_fast64_t bits = action->branches;
//     bool newCapture = false;
//     while (bits) {
//         int branch = bitscan(bits);
//         bits ^= (1llu << branch);
//         for (int i = 0; i < 2; i++) {
//             int square = EDGE_ADJACENT_SQUARES[branch][i];
//             if (square < 0) break;
//             newCapture = State_updateCaptured(state, square) || newCapture;
//         }
//     }
//     if (newCapture) {
//         state->score[turn] = popcount(state->captured[turn]);
//     }
// 
//     // Check for largest network changes
//     if (action->branches) {
//         int networkSize = State_largestNetworkSize(state, turn);
//         // If we've got a new largest network size
//         if (networkSize > state->largestNetworkSize) {
//             state->largestNetworkSize = networkSize;
//             // If the moving player didn't already have the largest ntwork
//             if (state->largestNetworkPlayer != turn) {
//                 state->largestNetworkPlayer = turn;
//                 // If the other player previously had the largest
//                 // network, lower their score
//                 if (state->largestNetworkPlayer == !turn) {
//                     state->score[!turn] -= LARGEST_NETWORK_SCORE;
//                 }
//                 // Raise current player's score
//                 state->score[turn] += LARGEST_NETWORK_SCORE;
//             }
//         // If current player has tied the largest network size, and did
//         // not already have it
//         } else if (networkSize == state->largestNetworkSize
//                 && turn != state->largestNetworkPlayer) {
//             // If the other player had the largest network before, they
//             // no longer have it
//             if (state->largestNetworkPlayer == !turn) {
//                 state->score[!turn] -= LARGEST_NETWORK_SCORE;
//             }
//             state->largestNetworkPlayer = PLAYER_NONE;
//         }
//     }
// 
//     // Process nodes
//     state->nodes[state->turn] |= action->nodes;
//     int nodeCount = popcount(action->nodes);
// 
//     // Pay resources for nodes
//     // TODO Don't pay resources in place phase
//     state->resources[state->turn][YELLOW] -= nodeCount;
//     state->resources[state->turn][GREEN] -= nodeCount;
// 
//     state->score[state->turn] += nodeCount;
// 
//     // TODO Check for exhaustion
//     // TODO Update cache of resources/turn
// 
//     // Next player's turn
//     state->turn = !turn;
// }
// 
// 
// void State_undo(struct State *state, const struct Action *action) {
//     // Undo turn advance
//     state->turn = !state->turn;
//     enum Player turn = state->turn;
// 
//     // Undo trade
//     if (action->trade.active) {
//         for (int i = 0; i < TRADE_NUM; i++) {
//             state->resources[turn][action->trade.in[i]]++;
//         }
//         state->resources[turn][action->trade.out]--;
//     }
// 
//     // Undo branches
//     // TODO Could this be subtract?
//     state->branches[turn] &= ~action->branches;
//     int branchCount = popcount(action->branches);
// 
//     // Refund resources for branches
//     state->resources[turn][RED] += branchCount;
//     state->resources[turn][BLUE] += branchCount;
// 
//     // Check if any squares are no longer captured
//     uint_fast64_t bits = action->branches;
//     while (bits) {
//         int branch = bitscan(bits);
//         bits ^= (1llu << branch);
//         for (int i = 0; i < 2; i++) {
//             int square = EDGE_ADJACENT_SQUARES[branch][i];
//             if (square < 0) break;
//             State_updateCaptured(state, square);
//         }
//     }
//     // TODO Can/should we make this conditional?
//     state->score[turn] = popcount(state->captured[turn]);
// 
//     // Check for largest network changes
//     if (action->branches) {
//         int size1 = State_largestNetworkSize(state, PLAYER_1);
//         int size2 = State_largestNetworkSize(state, PLAYER_2);
//         if (size1 > size2) {
//             state->largestNetworkSize = size1;
//             state->largestNetworkPlayer = PLAYER_1;
//             state->score[PLAYER_1] += 2;
//         } else if (size2 > size1) {
//             state->largestNetworkSize = size2;
//             state->largestNetworkPlayer = PLAYER_2;
//             state->score[PLAYER_2] += 2;
//         } else {
//             state->largestNetworkSize = size1;
//             state->largestNetworkPlayer = PLAYER_NONE;
//         }
//     }
// 
//     // Undo nodes
//     state->nodes[state->turn] &= ~action->nodes;
//     int nodeCount = popcount(action->nodes);
// 
//     // Refund resources for nodes
//     state->resources[state->turn][YELLOW] += nodeCount;
//     state->resources[state->turn][GREEN] += nodeCount;
// 
//     // Update score
//     // TODO Can we do this without popcounting?
//     state->score[state->turn] += popcount(state->nodes[turn]);
// }
