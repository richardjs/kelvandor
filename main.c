#include <stdlib.h>
#include <time.h>

#include "state.h"

#include <stdio.h>


int main() {
    srand(time(NULL));
    struct State state;
    State_randomStart(&state);

    State_derive(&state);
    printf("%d %d", state.scores[0], state.scores[1]);
    State_print(&state);

    return 0;
}
