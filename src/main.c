#include <stdlib.h>
#include <time.h>

#include "mcts.h"
#include "state.h"

#include <stdio.h>
#include <sys/time.h>


int main(int argc, char *argv[]) {
    fprintf(stderr, "Kelvandor v.1a (built %s %s)\n", __DATE__, __TIME__);

    srand(time(NULL));

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <serialized board>\n", argv[0]);
        return 1;
    }

    fprintf(stderr, "Input: %s\n", argv[1]);
    if (!validStateString(argv[1])) {
        fprintf(stderr, "Invalid state string: %s\n", argv[1]);
        return 2;
    }

    struct State state;
    State_fromString(&state, argv[1]);

    struct timeval start;
    gettimeofday(&start, NULL);

    mcts(&state, NULL);

    struct timeval end;
    gettimeofday(&end, NULL);
    int duration = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;
    fprintf(stderr, "total time:\t%d ms\n", duration);

    return 0;
}
