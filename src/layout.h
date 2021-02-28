#ifndef LAYOUT_H
#define LAYOUT_H

#include "state.h"

/*
                 *0*  0  *1*

                  1  (0)  2

         *2*  3  *3*  4  *4*  5  *5*

          6  (1)  7  (2)  8  (3)  9

 *6* 10  *7* 11  *8* 12  *9* 13 *10* 14 *11*

 15  (4) 16  (5) 17  (6) 18  (7) 19  (8) 20

*12* 21 *13* 22 *14* 23 *15* 24 *16* 25 *17*

         26  (9) 27 (10) 28 (11) 29

        *18* 30 *19* 31 *20* 32 *21*

                 33 (12) 34

                *22* 35 *23*
*/


enum Direction {NORTH=0, SOUTH, EAST, WEST};


extern const int SQUARE_ADJACENT_SQUARES[NUM_SQUARES][4];
extern const uint_fast64_t SQUARE_ADJACENT_BRANCHES[NUM_SQUARES][4];
extern const uint_fast32_t SQUARE_ADJACENT_CORNERS[NUM_SQUARES];
extern const int CORNER_ADJACENT_EDGES[NUM_CORNERS][4];
extern const int EDGE_ADJACENT_SQUARES[NUM_EDGES][2];
extern const uint_fast64_t EDGE_ADJACENT_EDGES[NUM_EDGES];
extern const uint_fast32_t EDGE_ADJACENT_CORNERS[NUM_EDGES];
extern const int CORNER_ADJACENT_SQUARES[NUM_CORNERS][4];


#endif
