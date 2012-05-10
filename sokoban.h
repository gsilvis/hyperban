#ifndef __HYPERBAN__SOKOBAN_H
#define __HYPERBAN__SOKOBAN_H

#include <stddef.h>

#include "types.h"
#include "graph.h"

typedef struct {
  Graph *graph;
  size_t unsolved;
} Board;

typedef enum {
  RESULT_NO_MOVE_POSSIBLE = -1,
  RESULT_MOVE = 0,
  RESULT_PUSH = 1,
} move_result_t;

move_result_t perform_move (Board *g, Move move);



#endif /* __HYPERBAN__SOKOBAN_H */
