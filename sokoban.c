#include "sokoban.h"
#include "types.h"
#include "graph.h"

int perform_move (Board *b, Move move)
{
  Graph *old = b->graph;
  Graph *new = old;
  int i;
  for (i = 0; i < move; i++)
    new = new->rotate_r;
  new = new->adjacent;
  for (; i < 4; i++)
    new = new->rotate_r;


  if (new->tile->tile_type == TILE_TYPE_WALL)
    return -1; /* No move allowed */

  if (new->tile->agent == AGENT_NONE)
    {
      b->graph = new; /* Walk */
      return b;
    }


  /* There's a box, so we need to figure out what's behind it */

  Graph *behind = new;
  for (i = 0; i < move; i++)
    behind = behind->rotate_r;
  behind = behind->adjacent;

  if (behind->tile->tile_type == TILE_TYPE_WALL) 
    return b; /* Push blocked by wall. */

  if (behind->tile->tile_type == AGENT_BOX)
    return b; /* Push blocked by box. */

  /* There's nothing behind the box, so the box moves */

  new->inhabitant->agent = AGENT_NONE;
  behind->inhabitant->agent = AGENT_BOX;

  b->graph = new;
  return b; /* Push sucessful */
}
