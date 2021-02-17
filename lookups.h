#ifndef LOOKUPS_H
#define LOOKUPS_H

#include "state.h"


enum Direction{NORTH=0, SOUTH, EAST, WEST};


extern const int SQUARE_ADJACENT_SQUARES[NUM_SQUARES][4];
extern const uint_fast64_t SQUARE_ADJACENT_BRANCHES[NUM_SQUARES][4];
extern const int EDGE_ADJACENT_SQUARES[NUM_EDGES][2];
extern const uint_fast64_t EDGE_ADJACENT_EDGES[NUM_EDGES];
extern const uint_fast32_t EDGE_ADJACENT_CORNERS[NUM_EDGES];


#endif
