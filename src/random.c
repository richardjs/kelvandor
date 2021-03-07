#include <stdio.h>
#include <stdlib.h>

#include "random.h"
#include "state.h"


void randomMoves(const struct State *initial) {
    struct State state = *initial;

    while (state.turn == initial->turn && state.actionCount) {
        struct Action action = state.actions[rand() % state.actionCount];

        char actionString[ACTION_STRING_SIZE];
        Action_toString(&action, actionString);
        printf("%s\n", actionString);

        State_act(&state, &action);
    }
}
