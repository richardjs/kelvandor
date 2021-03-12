#include <stdlib.h>
#include <time.h>

#include "mcts.h"
#include "state.h"

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    fprintf(stderr, "Kelvandor v0.2a (built %s %s)\n", __DATE__, __TIME__);

    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    fprintf(stderr, "Host: %s\n", hostname);


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

    fprintf(stderr, "Input state:\n");
    State_print(&state);

    if (state.actionCount == 0) {
        fprintf(stderr, "No actions from state\n");
        return 0;
    }

    struct MCTSResults results;
    struct MCTSOptions options;
    MCTSOptions_default(&options);
    mcts(&state, &results, &options);

    fprintf(stderr, "time:\t\t%ld ms\n", results.stats.duration);
    fprintf(stderr, "iterations:\t%ld\n", results.stats.iterations);
    fprintf(stderr, "iters/s:\t%ld\n",
        1000 * results.stats.iterations / results.stats.duration);

    fprintf(stderr, "action\tvalue\tvisits\titers\n");
    for (int i = 0; i < results.actionCount; i++) {
        char actionString[ACTION_STRING_SIZE];
        Action_toString(&results.actions[i], actionString);
        printf("%s\n", actionString);

        fprintf(stderr, "%s\t%.3f\t%ld\t%ld\n",
            actionString,
            results.actionStats[i].value / results.actionStats[i].visits,
            results.actionStats[i].visits,
            results.actionStats[i].iterations);
    }

    return 0;
}
