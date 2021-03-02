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
                printf("A square incorrectly marked captured: sq=%d captured[0]=0x%x\n", i, state.captured[0]);
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
            State_updateCaptured(&state, i);
            if (state.captured[0]) {
                printf("state.captured[0] shouldn't have anything: %x\n", state.captured[0]);
            }
            if (i == 2 || i == 5 || i == 6) {
                if (state.captured[1] != 0b1100100) {
                    printf("2, 5, and 6 should be captured: %x\n", state.captured[1]);
                }
            } else {
                if (state.captured[1] != 0) {
                    printf("nothing should be captured: %x\n", state.captured[1]);
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


    // Test start actions and undo
    {
        State_randomStart(&state);
        if (state.actionCount != 72) {
            printf("Incorrect number of starting actions: %d\n", state.actionCount);
        }
        struct Action action1 = state.actions[rand() % state.actionCount];
        struct State state1 = state;
        State_act(&state, &action1);
        if (state.turn != PLAYER_2) {
            printf("Wrong player turn during start places");
        }
        struct Action action2 = state.actions[rand() % state.actionCount];
        struct State state2 = state;
        State_act(&state, &action2);
        if (state.turn != PLAYER_2) {
            printf("Wrong player turn during start places");
        }
        struct Action action3 = state.actions[rand() % state.actionCount];
        struct State state3 = state;
        State_act(&state, &action3);
        if (state.turn != PLAYER_1) {
            printf("Wrong player turn during start places");
        }
        struct Action action4 = state.actions[rand() % state.actionCount];
        struct State state4 = state;
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

        State_undo(&state, &action4);
        if (memcmp(&state, &state4, sizeof(struct State)) != 0) {
           printf("Undo START_PLACE 4 results in different state\n");
           printf("Undone version:\n");
           State_printDetail(&state);
           printf("Reference version::\n");
           State_printDetail(&state4);
           printf("---\n");
        }
        State_undo(&state, &action3);
        if (memcmp(&state, &state3, sizeof(struct State)) != 0) {
           printf("Undo START_PLACE 3 results in different state\n");
           printf("Undone version:\n");
           State_printDetail(&state);
           printf("Reference version::\n");
           State_printDetail(&state3);
           printf("---\n");
        }
        State_undo(&state, &action2);
        if (memcmp(&state, &state2, sizeof(struct State)) != 0) {
           printf("Undo START_PLACE 2 results in different state\n");
           printf("Undone version:\n");
           State_printDetail(&state);
           printf("Reference version::\n");
           State_printDetail(&state2);
           printf("---\n");
        }
        State_undo(&state, &action1);
        if (memcmp(&state, &state1, sizeof(struct State)) != 0) {
            printf("Undo START_PLACE 1 results in different state\n");
            printf("Undone version:\n");
            State_printDetail(&state);
            printf("Reference version::\n");
            State_printDetail(&state1);
            printf("---\n");
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


    // Test BRANCH action undo
    {
        State_pastStart(&state);
        state.resources[PLAYER_2][RED] = 1;
        state.resources[PLAYER_2][BLUE] = 1;
        State_deriveActions(&state);
        for (int i = 0; i < state.actionCount; i++) {
            if (state.actions[i].type == BRANCH) {
                struct State test = state;
                struct Action action = test.actions[i];
                State_act(&test, &action);
                State_undo(&test, &action);
                if (memcmp(&state, &test, sizeof(struct State)) != 0) {
                    printf("Branch undo results in different state\n");
                    printf("Undone version:\n");
                    State_printDetail(&test);
                    printf("Reference version::\n");
                    State_printDetail(&state);
                    printf("---\n");
                    break;
                }
            }
        }
    }


    // Test TRADE action undo
    {
        State_pastStart(&state);
        for (int i = 0; i < state.actionCount; i++) {
            if (state.actions[i].type != TRADE) {
                continue;
            }
            struct State test = state;
            struct Action action = test.actions[i];
            State_act(&test, &test.actions[i]);
            State_undo(&test, &action);
            if (memcmp(&state, &test, sizeof(struct State)) != 0) {
                printf("Trade undo results in different state\n");
                printf("Undone version:\n");
                State_printDetail(&test);
                printf("Reference version::\n");
                State_printDetail(&state);
                printf("---\n");
                break;
            }
        }
    }


    // Test NODE action undo
    {
        bool foundNodeAction = false;
        while (!foundNodeAction) {
            State_pastStart(&state);
            for (int i = 0; i < state.actionCount; i++) {
                if (state.actions[i].type == NODE) {
                    foundNodeAction = true;
                    break;
                }
            }
        }

        for (int i = 0; i < state.actionCount; i++) {
            if (state.actions[i].type != NODE) continue;
            struct State test = state;
            struct Action action = test.actions[i];
            State_act(&test, &test.actions[i]);
            State_undo(&test, &action);
            if (memcmp(&state, &test, sizeof(struct State)) != 0) {
                printf("Node undo results in different state\n");
                printf("Undone version:\n");
                State_printDetail(&test);
                printf("Reference version::\n");
                State_printDetail(&state);
                printf("---\n");
                break;
            }
        }
    }


    // Test END action undo
    {
        State_pastStart(&state);

        for (int i = 0; i < state.actionCount; i++) {
            if (state.actions[i].type != END) continue;
            struct State test = state;
            struct Action action = test.actions[i];
            State_act(&test, &test.actions[i]);
            State_undo(&test, &action);
            if (memcmp(&state, &test, sizeof(struct State)) != 0) {
                printf("End undo results in different state\n");
                printf("Undone version:\n");
                State_printDetail(&test);
                printf("Reference version::\n");
                State_printDetail(&state);
                printf("---\n");
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

        // Setting up for making a capturing (and blocking) move so we can undo it
        state.branches[PLAYER_1] = 0b1000110000110;
        state.resources[PLAYER_1][RED] = 1;
        state.resources[PLAYER_1][BLUE] = 1;
        state.turn = PLAYER_1;
        State_derive(&state);
        for (int i = 0; i < state.actionCount; i++) {
            if (state.actions[i].type != BRANCH || state.actions[i].data != 0) {
                continue;
            }
            struct State test = state;
            struct Action action = test.actions[i];
            State_act(&test, &test.actions[i]);
            State_undo(&test, &action);
            if (memcmp(&state, &test, sizeof(struct State)) != 0) {
                printf("Capture undo results in different state\n");
                printf("Undone version:\n");
                State_printDetail(&test);
                printf("Reference version::\n");
                State_printDetail(&state);
                printf("---\n");
                break;
            }
        }
    }


    {
        // Test a whole bunch of actions, undos, and derives
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
                    printf("Guide:\trlrlrlrlrlrlrlrlrlrlrlrlrlnnnnnnnnnnnnnnnnnnnnnnnneeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeetbbggrryybbggrryyt\n");

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

                    return 1;
                }
            }

            while (size > 1) {
                size--;
                State_undo(&state, &actions[size]);
                if (memcmp(&state, &states[size], sizeof(struct State)) != 0) {
                    printf("Undo in random moves results in different state\n");
                    printf("Undone version:\n");
                    State_printDetail(&state);
                    printf("Reference version::\n");
                    State_printDetail(&states[size]);
                    printf("Undone action %d\tdata %d\n", actions[size].type, actions[size].data);
                    printf("---\n");
                    i = 1000;
                    break;
                }

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


    printf("Done\n");
    return 0;
}   
