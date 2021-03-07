#include <stdlib.h>
#include <time.h>

#include "random.h"
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

    randomMoves(&state);

    return 0;
}
