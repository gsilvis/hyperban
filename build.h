#ifndef __HYPERBAN__BUILD_H
#define __HYPERBAN__BUILD_H

#include "graph.h"

/* Give this function a list of SavedTiles, and how many tiles long it is.
   There are few things that MUST be followed for this to work.

   First of all, each path to a file must ONLY walk through tiles that have
   ALREADY been placed.
 */

static Graph *build_graph (SavedTile *tiles, size_t num_tiles);

#endif /* __HYPERBAN__BUILD_H */
