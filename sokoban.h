#ifndef __HYPERBAN__SOKOBAN_H
#define __HYPERBAN__SOKOBAN_H

#include "types.h"

typedef struct {
  Graph *graph;
  size_t unsolved;
} Board;

int perform_move (Board *g, Move move);



#endif /* __HYPERBAN__SOKOBAN_H */
