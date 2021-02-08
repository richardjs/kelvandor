#include "state.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() {
    printf("Kelvandor tests\n");

    time_t seed = time(NULL);
    printf("Seed: %ld\n", seed);
    srand(seed);

    printf("Running tests..\n");

    struct State state;
    State_randomStart(&state);

    state.nodes[0] = 0xf;
    state.nodes[1] = 0x10;
    State_derive(&state);
    if(state.score[0] != 4 && state.score[1] != 1){
        printf("Incorrect score: %d %d\n", state.score[0], state.score[1]);
    }


    printf("Done\n");
    return 0;
}   
