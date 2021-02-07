#include <stdlib.h>
#include <time.h>

#include "state.h"


int main() {
    srand(time(NULL));
    struct State state;
    State_randomStart(&state);
    State_print(&state);

    return 0;
}
