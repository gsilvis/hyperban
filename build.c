#include "types.h"
#include "build.h"
#include <malloc.h>



Graph *build_initial_node ()
{
  Graph (*first)[4] = malloc(4 * sizeof(Graph));
  for (int i = 0; i < 4; i++)
    first[i]->rotate_r = first[(i+1) % 4];
  Tile *t = malloc(sizeof(Tile));
  for (int i = 0; i < 4; i++)
    first[i]->tile = t;
  return first[0];
}

void build_enforce_convexity_left (Graph *g)
{
  if (!g)
    return; /* Go away */


  Graph *clockwise = g->rotate_r->rotate_r->rotate_r->adjacent;
  Graph *ccwise = g->adjacent;

  if (!clockwise || !ccwise)
    return; /* No need to change anything */


  /* There are THREE present nodes around this vertex, so we want to fill in
     the other two. */
  Graph *newclock = build_initial_node();
  Graph *newcc = build_initial_node();
  /* Link clockwise to newclock */
  clockwise->rotate_r->adjacent = newclock;
  newclock->adjacent = clockwise->rotate_r;
  /* Lock ccwise to newcc */
  ccwise->rotate_r->rotate_r->rotate_r->adjacent = newcc;
  newcc->adjacent = ccwise->rotate_r->rotate_r->rotate_r;
  /* Link newclock to newcc */
  newclock->rotate_r->adjacent = newcc->rotate_r->rotate_r->rotate_r;
  newcc->rotate_r->rotate_r->rotate_r->adjacent = newclock->rotate_r;

  /* recurse counterclockwise around the perimeter of the graph */
  build_enforce_convexity_left(clockwise);
}

void build_enforce_convexity_right (Graph *g)
{
  if (!g)
    return; /* Go away */


  Graph *clockwise = g->adjacent;
  Graph *ccwise = g->rotate_r->rotate_r->rotate_r->adjacent;

  if (!clockwise || !ccwise)
    return; /* No need to change anything */


  /* There are THREE present nodes around this vertex, so we want to fill in
     the other two. */
  Graph *newclock = build_initial_node();
  Graph *newcc = build_initial_node();
  /* Link clockwise to newclock */
  clockwise->rotate_r->adjacent = newclock;
  newclock->adjacent = clockwise->rotate_r;
  /* Lock ccwise to newcc */
  ccwise->rotate_r->rotate_r->rotate_r->adjacent = newcc;
  newcc->adjacent = ccwise->rotate_r->rotate_r->rotate_r;
  /* Link newclock to newcc */
  newclock->rotate_r->adjacent = newcc->rotate_r->rotate_r->rotate_r;
  newcc->rotate_r->rotate_r->rotate_r->adjacent = newclock->rotate_r;

  /* recurse clockwise around the perimeter of the graph */
  build_enforce_convexity_right(ccwise);
}


void build_add_node (Graph *graph, SavedTile *tile)
{
  char *direction_ptr = tile->path;

  if (!direction_ptr)
    return; /* If direction_ptr is null, go away. */

  for (; *direction_ptr; direction_ptr++)
    {
      switch (*direction_ptr)
        {
        case 'L': case 'l':
          graph = graph->rotate_r;
          /* Fall through */
        case 'B': case 'b':
          graph = graph->rotate_r;
          /* Fall through. */
        case 'R': case 'r':
          graph = graph->rotate_r;
          break;
        case 'F': case 'f':

          if (!graph->adjacent)
            {
              graph->adjacent = build_initial_node();
              graph->adjacent->adjacent = graph;
              build_enforce_convexity_left(graph);
              build_enforce_convexity_right(graph);
            }

          graph = graph->adjacent->rotate_r->rotate_r;
          break;
        }
    }





}



Graph *build_graph (SavedTile *tiles, size_t num_tiles)
{
  Graph *g = build_initial_node();
  for (size_t i = 0; i < num_tiles; i++)
    build_add_node(g, &tiles[i]);
  return g;
}


