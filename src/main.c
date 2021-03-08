#include <stdlib.h>
#include <time.h>

#include "mcts.h"
#include "state.h"

#include <stdio.h>


int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <serialized board>\n", argv[0]);
        return 1;
    }
    if (!validStateString(argv[1])) {
        fprintf(stderr, "Invalid state string: %s\n", argv[1]);
        return 2;
    }

    struct State state;
    State_fromString(&state, argv[1]);

    mcts(&state);

    return 0;
}
