#ifndef __HYPERBAN__GRAPH_H
#define __HYPERBAN__GRAPH_H

#include "types.h"

struct graph_t {
  struct graph_t *rotate_f;
  struct graph_t *adjacent;
  Tile *tile;
};

typedef struct graph_t Graph;




#endif /* __HYPERBAN__GRAPH_H */
