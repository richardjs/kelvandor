#include "state.h"

#include <stdio.h>


void State_print(const struct State *state) {
    char nc[NUM_CORNERS];
    for (int i = 0; i < NUM_CORNERS; i++) {
        char c = '.';
        if ((1llu << i) & state->nodes[PLAYER_1]) {
            c = 'X';
        }
        if ((1llu << i) & state->nodes[PLAYER_2]) {
            if (c == 'X') {
                c = '?';
            } else {
                c = 'O';
            }
        }
        nc[i] = c;
    }

    char ec[NUM_EDGES];
    for (int i = 0; i < NUM_EDGES; i++) {
        char c = ' ';
        if ((1llu << i) & state->branches[PLAYER_1]) {
            c = 'x';
        }
        if ((1llu << i) & state->branches[PLAYER_2]) {
            if (c == 'X') {
                c = '?';
            } else {
                c = 'o';
            }
        }
        ec[i] = c;
    }

    const char RESOURCE_CHARS[] = {'R', 'Y', 'B', 'G'};
    char rc[NUM_SQUARES];
    char lc[NUM_SQUARES];
    for (int i = 0; i < NUM_SQUARES; i++) {
        rc[i] = RESOURCE_CHARS[state->squares[i].resource];
        lc[i] = '0' + state->squares[i].limit;
        if (lc[i] == '0') {
            rc[i] = ' ';
            lc[i] = ' ';
        }
    }

    fprintf(stderr, "      %c%c%c%c\n", nc[0], ec[0], ec[0], nc[1]);
    fprintf(stderr, "      %c%c%c%c\n", ec[1], rc[0], lc[0], ec[2]);
    fprintf(stderr, "   %c%c%c%c%c%c%c%c%c%c\n", nc[2], ec[3], ec[3], nc[3], ec[4], ec[4], nc[4], ec[5], ec[5], nc[5]);
    fprintf(stderr, "   %c%c%c%c%c%c%c%c%c%c\n", ec[7], rc[1], lc[1], ec[7], rc[2], lc[2], ec[8], rc[3], lc[3], ec[9]);
    fprintf(stderr, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", nc[6], ec[10], ec[10], nc[7], ec[11], ec[11], nc[8], ec[12], ec[12], nc[9], ec[13], ec[13], nc[10], ec[14], ec[14], nc[11]);
    fprintf(stderr, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", ec[15], rc[4], lc[4], ec[16], rc[5], lc[5], ec[17], rc[6], lc[6], ec[18], rc[7], lc[7], ec[19], rc[8], lc[8], ec[20]);
    fprintf(stderr, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", nc[12], ec[21], ec[21], nc[13], ec[22], ec[22], nc[14], ec[23], ec[23], nc[15], ec[24], ec[24], nc[16], ec[25], ec[25], nc[17]);
    fprintf(stderr, "   %c%c%c%c%c%c%c%c%c%c\n", ec[26], rc[9], lc[9], ec[27], rc[10], lc[10], ec[28], rc[11], lc[11], ec[29]);
    fprintf(stderr, "   %c%c%c%c%c%c%c%c%c%c\n", nc[18], ec[30], ec[30], nc[19], ec[31], ec[31], nc[20], ec[32], ec[32], nc[21]);
    fprintf(stderr, "      %c%c%c%c\n", ec[33], rc[12], lc[12], ec[34]);
    fprintf(stderr, "      %c%c%c%c\n", nc[22], ec[35], ec[35], nc[23]);
}


void State_printDetail(const struct State *state) {
	State_print(state);
	fprintf(stderr, "Phase:\t%d\n", state->phase);
	fprintf(stderr, "Turn:\t%d\n", state->turn);

	for (enum Player player = 0; player < NUM_PLAYERS; player++) {
		fprintf(stderr, "Player %d:", player);
		fprintf(stderr, "\tScore:\t%d", state->score[player]);
		fprintf(stderr, "\tNodes:\t%d", popcount(state->nodes[player]));
		fprintf(stderr, "\tCaptured:\t%x\n", state->captured[player]);

		for (enum Resource resource = 0; resource < NUM_RESOURCES; resource++) {
		fprintf(stderr, "\tResource %d: %d (+%d)",
			resource, state->resources[player][resource],
			state->income[player][resource]);
		}
		fprintf(stderr, "\n");
	}
}
