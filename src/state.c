#include "state.h"
#include "layout.h"

#include <stdlib.h>
#include <string.h>


int State_largestNetworkSize(const struct State *state, enum Player player) {
    uint64_t networks[NUM_SQUARES] = {0llu};
    uint64_t networkAdjacents[NUM_SQUARES] = {0llu};
    int count = 0;

    uint64_t branches = state->branches[player];
    while (branches) {
        int branch = bitscan(branches);
        branches ^= (1llu << branch);

        if (count == 0) {
            networks[count] = (1llu << branch);
            networkAdjacents[count] = EDGE_ADJACENT_EDGES[branch];
            count++;
        } else {
            for (int i = 0; i < count; i++) {
                if ((1llu << branch) & networkAdjacents[i]) {
                    networks[i] |= (1llu << branch);
                    networkAdjacents[i] |= EDGE_ADJACENT_EDGES[branch];
                    goto nextbit;
                }
            }

            networks[count] = (1llu << branch);
            networkAdjacents[count] = EDGE_ADJACENT_EDGES[branch];
            count++;
        }

        nextbit:
        continue;
    }

    int largestSize = 0;
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (networkAdjacents[i] & networks[j]) {
                networks[i] |= networks[j];
                networkAdjacents[i] |= networkAdjacents[j];
            }
        }

        int networkSize = popcount(networks[i]);
        if (networkSize > largestSize) {
            largestSize = networkSize;
        }
    }

    return largestSize;
}


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

    // Tracks what player might own the searched region. Updated when we
    // find a bordering branch.  If we find bordering branches owned by
    // both players, no one captures the region.
    enum Player player = PLAYER_NONE;

    while (size) {
        int sq = stack[--size];
        crumbs[sq] = true;

        // Check each direction for a branch
        for (enum Direction dir = NORTH; dir < 4; dir++) {
            uint_fast64_t branch = (1llu << SQUARE_ADJACENT_BRANCHES[sq][dir]);

            // If P1 has a branch in this direction...
            if (state->branches[PLAYER_1] & branch) {
                // If player is still PLAYER_NONE, now we're checking if
                // P1 owns the region
                if (player == PLAYER_NONE) {
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
                if (player == PLAYER_NONE) {
                    player = PLAYER_2;
                } else if (player == PLAYER_1) {
                    goto nocapture;
                }
            // If there is not a branch in this direction, see if
            // there's another square that may be part of a larger
            // region
            // (This is the branch that continues the search)
            } else if (SQUARE_ADJACENT_SQUARES[sq][dir] >= 0) {
                int adjacent = SQUARE_ADJACENT_SQUARES[sq][dir];
                if (!crumbs[adjacent]) {
                    stack[size++] = adjacent;
                }
            // If there's no branches in this direction and not another
            // square, we must be on the edge of the board (without a
            // bounding branch), and thus this region is not captured
            } else {
                // Have to put this back on so we can search it later
                // TODO once we break this function into act and undo versions,
                // this doesn't need to be in the act
                stack[size++] = sq;
                goto nocapture;
            }
        }
    }

    // If we've gotten here, the searched squares are a captured region;
    // mark them all as captured by player
    for (int i = 0; i < NUM_SQUARES; i++) {
        if (crumbs[i]) {
            state->squares[i].captor = player;
            state->captured[player] |= (1llu << i);

            for (enum Direction dir = 0; dir < 4; dir++) {
                int adjacent = SQUARE_ADJACENT_SQUARES[i][dir];
                if (adjacent < 0) {
                    continue;
                }
                if (state->squares[adjacent].captor == player || crumbs[adjacent]) {
                    state->blocked[!player] |= (1llu << SQUARE_ADJACENT_BRANCHES[i][dir]);
                }
            }
        }
    }

    return true;

    nocapture:
    // Normally we could leave state alone if there are no captures.
    // However, we want to support undo, and thus we'll need to mark
    // the squares we visited as uncaptured (since they may have
    // been captured by a move being undone).
    // TODO It might be worth it to create a separate version of
    // this function, or by some other means skip this processing
    // when not undoing (especially since we'll never capture by
    // undoing either).
    // We'll also have to finish our above search, because it could
    // terminate before putting a crumb on all connected nodes (some
    // of which may have been marked captured by the undone action)
    while (size) {
        int sq = stack[--size];
        crumbs[sq] = true;

        for (enum Direction dir = NORTH; dir < 4; dir++) {
            uint_fast64_t branch = (1llu << SQUARE_ADJACENT_BRANCHES[sq][dir]);
            if (state->branches[PLAYER_1] & branch) {
            } else if (state->branches[PLAYER_2] & branch) {
            } else if (SQUARE_ADJACENT_SQUARES[sq][dir] >= 0) {
                int adjacent = SQUARE_ADJACENT_SQUARES[sq][dir];
                if (!crumbs[adjacent]) {
                    stack[size++] = adjacent;
                }
            }
        }
    }

    for (int i = 0; i < NUM_SQUARES; i++) {
        if (!crumbs[i]) {
            continue;
        }
        for (enum Player p = 0; p < NUM_PLAYERS; p++) {
            state->captured[p] &= ~(1llu << i);
            state->squares[i].captor = PLAYER_NONE;
        }
        for (enum Direction dir = 0; dir < 4; dir++) {
            for (enum Player p = 0; p < NUM_PLAYERS; p++) {
                state->blocked[p] &= ~(1llu << SQUARE_ADJACENT_BRANCHES[i][dir]);
            }
        }
    }

    return false;
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


void State_updateLargestNetworkScore(struct State *state) {
    // Reset largest network information
    if (state->largestNetworkPlayer != PLAYER_NONE) {
        state->score[state->largestNetworkPlayer] -= LARGEST_NETWORK_SCORE;
    }
    state->largestNetworkSize = 0;
    state->largestNetworkPlayer = PLAYER_NONE;

    for (enum Player player = 0; player < NUM_PLAYERS; player++) {
        int networkSize = State_largestNetworkSize(state, player);
        if (networkSize > state->largestNetworkSize) {
            state->largestNetworkSize = networkSize;
            state->largestNetworkPlayer = player;
        } else if (networkSize == state->largestNetworkSize) {
            state->largestNetworkSize = networkSize;
            state->largestNetworkPlayer = PLAYER_NONE;
        }
    }

    if (state->largestNetworkPlayer != PLAYER_NONE) {
        state->score[state->largestNetworkPlayer] += LARGEST_NETWORK_SCORE;
    }
}


void State_deriveStartActions(struct State *state) {
    state->actionCount = 0;

    uint_fast64_t openEdges = ~(state->branches[PLAYER_1] | state->branches[PLAYER_2]);
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
    if (state->score[PLAYER_1] >= WIN_SCORE || state->score[PLAYER_2] >= WIN_SCORE) {
        state->actionCount = 0;

        // TODO only do this with test flag
        memset(&state->actions[state->actionCount], 0,
            sizeof(struct Action) * (MAX_ACTIONS - state->actionCount));

        return;
    }

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
    uint_fast64_t openEdges = ~state->branches[PLAYER_1] & ~state->branches[PLAYER_2] & ~state->blocked[state->turn];
    openEdges &= (1llu << NUM_EDGES) - 1;
    adjacentEdges &= openEdges;
    uint_fast32_t openCorners = ~state->nodes[PLAYER_1] & ~state->nodes[PLAYER_2];
    openCorners &= (1llu << NUM_CORNERS) - 1;
    adjacentCorners &= openCorners;

    if (state->resources[state->turn][RED >= BRANCH_COST]
            && state->resources[state->turn][BLUE] >= BRANCH_COST) {
        bits = adjacentEdges;
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
        bits = adjacentCorners;
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

    // Remaining limit on squares
    uint_fast32_t combinedNodes = state->nodes[PLAYER_1] | state->nodes[PLAYER_2];
    for (int i = 0; i < NUM_SQUARES; i++) {
        struct Square *square = &state->squares[i];
        square->remaining = square->limit;
        square->remaining -= popcount(combinedNodes & SQUARE_ADJACENT_CORNERS[i]);
    }

    // Captured squares
    for (int i = 0; i < NUM_SQUARES; i++ ) {
        State_updateCaptured(state, i);
    }
    for (enum Player player = 0; player < NUM_PLAYERS; player++) {
        state->score[player] += popcount(state->captured[player]);
    }

    state->largestNetworkPlayer = PLAYER_NONE;
    State_updateLargestNetworkScore(state);

    State_deriveActions(state);
}


void State_act(struct State *state, const struct Action *action) {
    switch (action->type) {
        case START_PLACE: {
            int node = action->data & 0b11111;
            int dir = action->data >> 6;
            int branch = CORNER_ADJACENT_EDGES[node][dir];
            state->nodes[state->turn] |= (1llu << node);
            state->branches[state->turn] |= (1llu << branch);

            state->score[state->turn] += 1;

            for (int i = 0; i < 4 && CORNER_ADJACENT_SQUARES[node][i] >= 0; i++) {
                state->squares[CORNER_ADJACENT_SQUARES[node][i]].remaining--;
            }

            if (state->turn == PLAYER_1) {
                state->turn = PLAYER_2;
                if (popcount(state->nodes[PLAYER_2]) == 2) {
                    State_collectResources(state, 1);
                }
            } else if (popcount(state->nodes[PLAYER_2]) == START_NODES) {
                state->turn = PLAYER_1;
            }

            State_updateLargestNetworkScore(state);

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

            state->score[state->turn] -= popcount(state->captured[state->turn]);
            for (int i = 0; i < 2; i++) {
                int square = EDGE_ADJACENT_SQUARES[action->data][i];
                if (square < 0) continue;
                State_updateCaptured(state, square);
            }
            state->score[state->turn] += popcount(state->captured[state->turn]);

            State_updateLargestNetworkScore(state);

            break;
        }

        case NODE: {
            state->resources[state->turn][YELLOW] -= 2;
            state->resources[state->turn][GREEN] -= 2;
            state->nodes[state->turn] |= (1llu << action->data);

            for (int i = 0; i < 4 && CORNER_ADJACENT_SQUARES[action->data][i] >= 0; i++) {
                state->squares[CORNER_ADJACENT_SQUARES[action->data][i]].remaining--;
            }

            state->score[state->turn] += 1;

            break;
        }

        case END: {
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

            state->score[state->turn] -= 1;

            for (int i = 0; i < 4 && CORNER_ADJACENT_SQUARES[node][i] >= 0; i++) {
                state->squares[CORNER_ADJACENT_SQUARES[node][i]].remaining++;
            }

            State_updateLargestNetworkScore(state);

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

            state->score[state->turn] -= popcount(state->captured[state->turn]);
            for (int i = 0; i < 2; i++) {
                int square = EDGE_ADJACENT_SQUARES[action->data][i];
                if (square < 0) continue;
                State_updateCaptured(state, square);
            }
            state->score[state->turn] += popcount(state->captured[state->turn]);

            State_updateLargestNetworkScore(state);

            break;
        }

        case NODE: {
            state->resources[state->turn][YELLOW] += 2;
            state->resources[state->turn][GREEN] += 2;
            state->nodes[state->turn] ^= (1llu << action->data);

            for (int i = 0; i < 4 && CORNER_ADJACENT_SQUARES[action->data][i] >= 0; i++) {
                state->squares[CORNER_ADJACENT_SQUARES[action->data][i]].remaining++;
            }

            state->score[state->turn] -= 1;

            break;
        }

        case END: {
            State_collectResources(state, -1);
            state->turn = !state->turn;

            break;
        }
    }

    State_deriveActions(state);
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
            } while (state->squares[try_place].remaining > 0);

            state->squares[try_place].resource = resource;
            state->squares[try_place].limit = limit;
            state->squares[try_place].remaining = limit;
            state->squares[try_place].captor = PLAYER_NONE;
        }
    }

    State_derive(state);
}
