#include "lookups.h"
#include "state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main() {
    printf("Kelvandor tests\n");

    time_t seed = time(NULL);
    printf("Seed: %ld\n", seed);
    srand(seed);

    printf("Running tests...\n");

    struct State state;
    State_randomStart(&state);

    state.nodes[0] = 0xf;
    state.nodes[1] = 0x10;
    State_derive(&state);
    if(state.score[0] != 4 && state.score[1] != 1){
        printf("Incorrect score: %d %d\n", state.score[0], state.score[1]);
    }


    State_randomStart(&state);
    for (int i = 0; i < NUM_SQUARES; i++) {
        State_updateCaptured(&state, i);
        if (state.captured[0] > 0 || state.captured[1] > 0) {
            printf("A square on a blank board marked captured: %d\n", i);
        }
    }

    // Basic test of updateCaptured for a single square
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

    // Testing captures of mutiple squares
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


    // Test basic undo
//    State_randomStart(&state);
//    struct State original = state;
//    struct Action action;
//    action.trade.active = true;
//    action.trade.in[0] = YELLOW;
//    action.trade.in[1] = RED;
//    action.trade.in[2] = BLUE;
//    action.trade.out = GREEN;
//    action.branches = 0b11111;
//    action.nodes = 0b101;
//    State_act(&state, &action);
//    State_undo(&state, &action);
//    if (memcmp(&state, &original, sizeof(struct State)) != 0) {
//       printf("Acting and undoing does not result in identical state\n");
//       State_printDetail(&state);
//       State_printDetail(&original);
//       printf("---\n");
//    }
//
//
//    // Test calculating network sizes
//    State_randomStart(&state);
//    state.branches[0] = 0b10000000000001;
//    int size = State_largestNetworkSize(&state, PLAYER_1);
//    if (size != 1) {
//        printf("Error calculating largest network size in simple case: %d\n", size);
//    }
//    state.branches[0] = 0b1000000000001000010011001000;
//    size = State_largestNetworkSize(&state, PLAYER_1);
//    if (size != 5) {
//        printf("Error calculating largest network size in more complex case: %d\n", size);
//    }
//
//    // TODO test one player taking the largest network from the other, and tying
//
//    // Test derive
//     State_randomStart(&state);
//     action.trade.active = true;
//     action.trade.in[0] = YELLOW;
//     action.trade.in[1] = RED;
//     action.trade.in[2] = BLUE;
//     action.trade.out = GREEN;
//     action.branches = 0b11111;
//     action.nodes = 0b101;
//     State_act(&state, &action);
//     original = state;
//     State_derive(&state);
//     if (memcmp(&state, &original, sizeof(struct State)) != 0) {
//        printf("Deriving results in different state\n");
//        State_printDetail(&state);
//        State_printDetail(&original);
//        printf("---\n");
//     }
// 
//     State_randomStart(&state);
//     state.resources[0][RED] = 8;
//     state.resources[0][BLUE] = 8;
//     state.resources[0][GREEN] = 8;
//     state.resources[0][YELLOW] = 8;
//     state.nodes[0] = 1;
//     state.branches[0] = 1;
//     State_actions(&state);


    // Test start actions
    State_randomStart(&state);
    if (state.actionCount != 72) {
        printf("Incorrect number of starting actions\n");
    }
    State_act(&state, &state.actions[rand() % state.actionCount]);
    if (state.turn != PLAYER_2) {
        printf("Wrong player turn during start places");
    }
    State_act(&state, &state.actions[rand() % state.actionCount]);
    if (state.turn != PLAYER_2) {
        printf("Wrong player turn during start places");
    }
    State_act(&state, &state.actions[rand() % state.actionCount]);
    if (state.turn != PLAYER_1) {
        printf("Wrong player turn during start places");
    }
    State_act(&state, &state.actions[rand() % state.actionCount]);
    if (state.turn != PLAYER_2) {
        printf("Wrong player turn during start places");
    }
    for (int i = 0; i < state.actionCount; i++) {
        if (state.actions[i].type == START_PLACE) {
            printf("START_PLACE move after start phase");
            break;
        }
    }
    State_print(&state);


    printf("Done\n");

    return 0;
}   
