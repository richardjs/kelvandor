#include <stdlib.h>
#include <time.h>

#include "mcts.h"
#include "state.h"

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>


void printBanner(FILE* stream)
{
    fprintf(stream, "Kelvandor v0.2a (built %s %s)\n", __DATE__, __TIME__);

    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    fprintf(stream, "Host: %s\n", hostname);
}


void printStart()
{
    struct State state;
    State_randomStart(&state);

    char stateString[STATE_STRING_SIZE];
    State_toString(&state, stateString);
    printf("%s\n", stateString);
}


void printUsage(char name[])
{
    fprintf(stderr, "Usage: %s [options] serialized board\n\
Options:\n\
    -c UCTC\t\tMCTS UCTC parameter, controlling exploration vs. exploitation\n\
    -g\t\t\tgenerate a starting state string\n\
    -i iterations\tnumber of MCTS iterations per action\n\
    -s\t\t\tsingle action mode\n\
    -v\t\t\tprint version information and exit\n", name);
}


int main(int argc, char *argv[])
{
    srand(time(NULL));

    struct MCTSOptions options;
    MCTSOptions_default(&options);
    int opt;
    while ((opt = getopt(argc, argv, "i:c:sgv")) != -1) {
        switch (opt) {
            case 'i':
                options.iterations = atoi(optarg);
                break;
            case 'c':
                options.uctc = atof(optarg);
                break;
            case 's':
                options.multiaction = false;
                break;
            case 'g':
                printStart();
                return 0;
            case 'v':
                printBanner(stdout);
                return 0;
            default:
                printUsage(argv[0]);
                return 1;
        }
    }
    if (argc == optind) {
        printUsage(argv[0]);
        return 1;
    }

    printBanner(stderr);

    fprintf(stderr, "Input: %s\n", argv[optind]);
    if (!validStateString(argv[optind])) {
        fprintf(stderr, "Invalid state string: %s\n", argv[optind]);
        return 2;
    }

    struct State state;
    State_fromString(&state, argv[optind]);

    fprintf(stderr, "Input state:\n");
    State_print(&state);
    fprintf(stderr, "\n");

    if (state.actionCount == 0) {
        fprintf(stderr, "No actions from state\n");
        return 0;
    }

    struct MCTSResults results;
    mcts(&state, &results, &options);

    fprintf(stderr, "time:\t\t%ld ms\n", results.stats.duration);
    fprintf(stderr, "iterations:\t%ld\n", results.stats.iterations);
    fprintf(stderr, "iters/s:\t%ld\n",
        results.stats.duration ?
            1000 * results.stats.iterations / results.stats.duration : 0);
    fprintf(stderr, "sim depth out:\t%.4g%%\n",
        results.stats.simulations ?
            100 * (float)results.stats.depthOuts / results.stats.simulations : 0);
    fprintf(stderr, "tree depth:\t%d\n", results.stats.treeDepth);
    fprintf(stderr, "tree size:\t%ld MiB\n",
        results.stats.treeBytes / 1024 / 1024);
    fprintf(stderr, "UCTC:\t\t%g\n", options.uctc);

    fprintf(stderr, "action\tvalue\tvisits\tbranch\ttime\titers\titers/s\n");
    for (int i = 0; i < results.actionCount; i++) {
        char actionString[ACTION_STRING_SIZE];
        Action_toString(&results.actions[i], actionString);
        printf("%s\n", actionString);

        fprintf(stderr, "%s\t%.3g\t%ld\t%d\t%ld\t%ld\t%ld\n",
            actionString,
            results.actionStats[i].visits ?
                results.actionStats[i].value / results.actionStats[i].visits : 0,
            results.actionStats[i].visits,
            results.actionStats[i].actionCount,
            results.actionStats[i].duration,
            results.actionStats[i].iterations,
            results.actionStats[i].duration ?
                1000 * results.actionStats[i].iterations
                / results.actionStats[i].duration : 0);
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "State after actions:\n");
    State_print(&results.state);
    char stateString[STATE_STRING_SIZE];
    State_toString(&results.state, stateString);
    fprintf(stderr, "%s\n", stateString);

    if (State_currentPlayerWon(&results.state)) {
        fprintf(stderr, "player %d wins\n", state.turn + 1);
    }

    return 0;
}
