#ifndef STANDALONE
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef uint_fast32_t nodebits;
typedef uint_fast64_t branchbits;

#define NUM_PLAYERS 2
#define NUM_RESOURCES 4
#define MAX_LIMIT 3
#define NUM_SQUARES (3*4 + 1)
#define NUM_CORNERS 24
#define NUM_EDGES 36

enum Player {PLAYER_1=0, PLAYER_2};
enum Resource {RED=0, YELLOW, BLUE, GREEN};


void bitboardToArray(uint_fast64_t bitboard, uint_fast8_t *array, size_t size) {
    for (int i = 0; i < size; i++) {
        array[i] = ((1llu << i) & bitboard) > 1;
    }
}


struct Square {
    enum Resource resource;
    uint_fast8_t limit;
};


struct State {
    // Unchanging information
    struct Square squares[NUM_SQUARES];

    // Core information
    nodebits nodes[2];
    branchbits branches[2];

    uint_fast8_t playerResources[NUM_PLAYERS][NUM_RESOURCES];

    enum Player turn;

    // Derived information
    uint_fast8_t playerScores[NUM_PLAYERS];   
};


void State_randomStart(struct State *state) {
    // Most fields start off at 0
    memset(state, 0, sizeof(struct State));
    
    // Randomize the starting squares by interating through all squares
    // and picking a random location for each. The vacant square will
    // be at the last location
    for (enum Resource resource = 0; resource < NUM_RESOURCES; resource++) {
        for (int limit = 1; limit <= MAX_LIMIT; limit ++){
            // Find a place that does not already have a square. All
            // squares but the vacant have a limit > 0.
            int try_place;
            do {
                try_place = rand() % NUM_SQUARES;
            } while (state->squares[try_place].limit > 0);

            state->squares[try_place].resource = resource;
            state->squares[try_place].limit = limit;
        }
    }
}


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


#ifndef STANDALONE


static PyObject *c_minimax(PyObject *self, PyObject *args) {
    return PyFloat_FromDouble(1.0);
}


static PyMethodDef CMethods[] = {
    {"minimax", c_minimax, METH_VARARGS, "Score a state with minimax."},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef cmodule = {
    PyModuleDef_HEAD_INIT,
    "c",
    NULL,
    -1,
    CMethods
};


PyMODINIT_FUNC PyInit_c(void) {
    return PyModule_Create(&cmodule);
}


#else


int main() {
    srand(time(NULL));
    struct State state;
    State_randomStart(&state);
    State_print(&state);

    return 0;
}


#endif
