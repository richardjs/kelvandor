#include "state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void State_derive(struct State *state);
void State_deriveActions(struct State *state);
bool State_updateCaptured(struct State *state, int square);
int State_largestNetworkSize(const struct State *state, enum Player player);
void State_collectResources(struct State *state, int sign);


void State_pastStart(struct State *state) {
    State_randomStart(state);
    while (state->actions[0].type == START_PLACE) {
        State_act(state, &state->actions[rand() % state->actionCount]);
    }
}


int main() {
    printf("Kelvandor tests\n");

    #ifdef __EMSCRIPTEN__
    printf("Emscripten detected\n");
    #endif

    time_t seed = time(NULL);
    printf("Seed: %ld\n", seed);
    srand(seed);

    printf("Running tests...\n");

    struct State state;

    // Test basic score derivation
    {
        State_randomStart(&state);
        state.nodes[0] = 0xf;
        state.nodes[1] = 0x10;
        State_derive(&state);
        if(state.score[0] != 4 && state.score[1] != 1){
            printf("Incorrect score: %d %d\n", state.score[0], state.score[1]);
        }
    }


    // Test updateCaptured on blank board
    {
        State_randomStart(&state);
        for (int i = 0; i < NUM_SQUARES; i++) {
            State_updateCaptured(&state, i);
            if (state.captured[0] > 0 || state.captured[1] > 0) {
                printf("A square on a blank board marked captured: %d\n", i);
            }
        }
    }


    // Basic test of updateCaptured for a single square
    {
        State_randomStart(&state);
        state.branches[0] = 0b10111;
        State_updateCaptured(&state, 0);
        if (state.captured[0] != 0x1) {
            printf("A square not marked captured, case 1: %d\n", 0);
        }
        for (int i = 1; i < NUM_SQUARES; i++) {
            state.captured[0] = 0x1;
            State_updateCaptured(&state, i);
            if (state.captured[0] != 0x1 || state.captured[1] > 0) {
                printf("A square incorrectly marked captured: sq=%d captured[0]=0x%lx\n", i, state.captured[0]);
                State_print(&state);
                printf("---\n");
            }
        }
    }

    // Testing captures of mutiple squares
    {
        State_randomStart(&state);
        state.branches[1] = 0b110001010000100110010000;
        for (int i = 0; i < NUM_SQUARES; i++) {
            state.captured[1] = 0;
            for (int sq = 0; sq < NUM_SQUARES; sq++) {
                state.squares[sq].captor = PLAYER_NONE;
            }
            State_updateCaptured(&state, i);
            if (state.captured[0]) {
                printf("state.captured[0] shouldn't have anything: %lx\n", state.captured[0]);
            }
            if (i == 2 || i == 5 || i == 6) {
                if (state.captured[1] != 0b1100100) {
                    printf("2, 5, and 6 should be captured: %lx\n", state.captured[1]);
                }
            } else {
                if (state.captured[1] != 0) {
                    printf("nothing should be captured: %lx\n", state.captured[1]);
                }
            }
        }
    }


    // Test calculating network sizes
    {
        State_randomStart(&state);
        state.branches[0] = 0b10000000000001;
        int size = State_largestNetworkSize(&state, PLAYER_1);
        if (size != 1) {
            printf("Error calculating largest network size in simple case: %d\n", size);
        }
        state.branches[0] = 0b1000000000001000010011001000;
        size = State_largestNetworkSize(&state, PLAYER_1);
        if (size != 5) {
            printf("Error calculating largest network size in more complex case: %d\n", size);
        }
    }


    // Test start actions
    {
        State_randomStart(&state);
        if (state.actionCount != 72) {
            printf("Incorrect number of starting actions: %d\n", state.actionCount);
        }
        struct Action action1 = state.actions[rand() % state.actionCount];
        State_act(&state, &action1);
        if (state.turn != PLAYER_2) {
            printf("Wrong player turn during start places");
        }
        struct Action action2 = state.actions[rand() % state.actionCount];
        State_act(&state, &action2);
        if (state.turn != PLAYER_2) {
            printf("Wrong player turn during start places");
        }
        struct Action action3 = state.actions[rand() % state.actionCount];
        State_act(&state, &action3);
        if (state.turn != PLAYER_1) {
            printf("Wrong player turn during start places");
        }
        struct Action action4 = state.actions[rand() % state.actionCount];
        State_act(&state, &action4);
        if (state.turn != PLAYER_2) {
            printf("Wrong player turn during start places");
        }
        for (int i = 0; i < state.actionCount; i++) {
            if (state.actions[i].type == START_PLACE) {
                printf("START_PLACE move after start phase\n");
                break;
            }
        }
    }


    // Test basic resource collection
    {
        State_randomStart(&state);
        state.nodes[PLAYER_1] = 0b11;
        state.nodes[PLAYER_2] = 0b11<<22;
        state.squares[0].resource = RED;
        state.squares[0].remaining = 3;
        State_collectResources(&state, 1);
        if (state.resources[PLAYER_1][RED] != 2) {
            printf("Basic collect resources test failed\n");
        }
    }


    // Test that two trades are not allowed
    // TODO This test was written assuming we're collecting resources
    {
        State_pastStart(&state);
        for (int i = 0; i < state.actionCount; i++) {
            if (state.actions[i].type != TRADE) {
                continue;
            }
            struct State test = state;
            State_act(&test, &test.actions[i]);
            bool fail = false;
            for (int j = 0; j < test.actionCount; j++) {
                if (test.actions[j].type == TRADE) {
                    printf("Two trade actions allowed in a turn\n");
                    fail = true;
                    break;
                }
            }
            if (fail) {
                break;
            }
        }
    }


    // Test captured region build blocking and undoing after blocking
    {
        State_randomStart(&state);
        state.nodes[PLAYER_1] = 0b1000000001;
        state.nodes[PLAYER_2] = 0b0100000100;
        state.branches[PLAYER_1] = 0b1000110000111;
        state.branches[PLAYER_2] = 0b0100001001000;
        state.turn = PLAYER_2;
        state.resources[PLAYER_2][RED] = 1;
        state.resources[PLAYER_2][BLUE] = 1;

        for (int i = 0; i < state.actionCount; i++) {
            if (state.actions[i].type != BRANCH) {
                continue;
            }
            if (state.actions[i].data == 4) {
                printf("Build possible in opponent-captured region\n");
                State_printDetail(&state);
            }
        }
    }


    {
        // Test a whole bunch of actions and derives
        // Also test serialization and deserialization
        for (int i = 0; i < 100; i++) {
            const int TEST_DEPTH = 200;

            struct Action actions[TEST_DEPTH];
            struct State states[TEST_DEPTH];
            int size = 0;

            State_randomStart(&state);
            while (state.actionCount && size < TEST_DEPTH) {
                states[size] = state;
                actions[size] = state.actions[rand() % state.actionCount];
                State_act(&state, &actions[size++]);

                // Check for correctness in captured fields
                for (int sq = 0; sq < NUM_SQUARES; sq++) {
                    if (state.squares[sq].captor == PLAYER_NONE) {
                        continue;
                    }
                    if (!(state.captured[state.squares[sq].captor] & (1llu << sq))) {
                        printf("square.captor is not present in state.captured\n");
                        return 3;
                    }
                }
                for (enum Player p = 0; p < NUM_PLAYERS; p++) {
                    uint_fast16_t bits = state.captured[p];
                    while (bits) {
                        int bit = bitscan(bits);
                        bits ^= (1llu << bit);

                        if (state.squares[bit].captor != p) {
                            printf("state.captured does not match square.captor\n");
                            return 4;
                        }
                    }
                }

                // While we've got a bunch of random states, test the serialization code
                char string[STATE_STRING_SIZE];
                State_toString(&state, string);
                struct State test = state;
                State_fromString(&test, string);

                if (memcmp(&state, &test, sizeof(struct State)) != 0) {
                    printf("Serializing and deserializing does not result in same state\n");
                    printf("Deserialized version:\n");
                    State_printDetail(&test);
                    printf("Reference version:\n");
                    State_printDetail(&state);
                    printf("String:\t%s\n", string);
                    printf("Guide:\trlrlrlrlrlrlrlrlrlrlrlrlrlnnnnnnnnnnnnnnnnnnnnnnnneeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeetggyybbrrggyybbrrt\n");

                    if (memcmp(&state.squares, &test.squares, NUM_SQUARES*sizeof(struct Square)) != 0) {
                        printf("Squares differ\n");
                        for (int sq = 0; sq < NUM_SQUARES; sq++) {
                            printf("square %d:\tres: %d:%d\tlim: %d:%d\trem: %d:%d\tcap: %d:%d\n",
                                sq,
                                test.squares[sq].resource, state.squares[sq].resource,
                                test.squares[sq].limit, state.squares[sq].limit,
                                test.squares[sq].remaining, state.squares[sq].remaining,
                                test.squares[sq].captor, state.squares[sq].captor);
                        }
                    }
                    if (memcmp(&state.actions, &test.actions, MAX_ACTIONS*sizeof(struct Action)) != 0) {
                        printf("Actions differ\n");
                    }

                    char actionString[ACTION_STRING_SIZE];
                    Action_toString(&actions[size-1], actionString);
                    printf("Last action:\t%s\n", actionString);

                    return 1;
                }

                for (int a = 0; a < state.actionCount; a++) {
                    char actionString[ACTION_STRING_SIZE];
                    Action_toString(&state.actions[a], actionString);
                    struct Action action;
                    Action_fromString(&action, actionString);

                    if (memcmp(&action, &state.actions[a], sizeof(struct Action)) != 0) {
                        printf("Action differs after serialization and deserialization: %s\n", actionString);
                        return 2;
                    }
                }
            }

            while (size > 1) {
                size--;

                struct State derived = states[size];
                State_derive(&derived);
                if (memcmp(&derived, &states[size], sizeof(struct State)) != 0) {
                    printf("Derive in random moves results in different state\n");
                    printf("Derived version:\n");
                    State_printDetail(&derived);
                    printf("Reference version::\n");
                    State_printDetail(&states[size]);
                    printf("---\n");
                    i = 1000;

                    if (memcmp(&derived.squares, &states[size].squares, sizeof(struct Square) * NUM_SQUARES) != 0) {
                        printf("Squares differ");
                    }

                    break;
                }
            }
        }
    }


    // Test state serialization and deserialization
    {
        State_randomStart(&state);

        char string[STATE_STRING_SIZE];
        State_toString(&state, string);
        struct State test = state;
        State_fromString(&test, string);
        if (memcmp(&state, &test, sizeof(struct State)) != 0) {
            printf("Serializing and deserializing does not result in same state\n");
        }
    }


    // Test for a bug allowing node builds without enough resources
    {
        State_fromString(&state, "r2g1b2r3g2y2v0g3y1b3r1b1y3000020000100001200000000000000002001120001000022000100000000201010000020002040");
        for (int i = 0; i < state.actionCount; i++) {
            if (state.actions[i].type == NODE) {
                printf("Build node action when not enough resources\n");
                State_print(&state);
                char actionString[ACTION_STRING_SIZE];
                Action_toString(&state.actions[i], actionString);
                printf("%s\n", actionString);
                return 5;
            }
        }
    }


    printf("Done\n");
    return 0;
}   
