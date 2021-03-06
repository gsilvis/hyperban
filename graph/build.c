/* Hyperban is an implementation of Sokoban on the hyperbolic plane.  Copyright
 * (C) 2012 George Silvis, III <george.iii.silvis@gmail.com> and Allan Wirth
 * <allan@allanwirth.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "types.h"
#include "build.h"
#include <malloc.h>

Graph *build_initial_node (void)
{
  Tile *t = malloc(sizeof(Tile));

  /* Set the Tile to the DEFAULT VALUES */
  t->tile_type = TILE_TYPE_DEFAULT;
  t->agent = AGENT_DEFAULT;
  t->search_flag = 0;

  Graph *nodes[4];
  for (size_t i = 0; i < 4; i++)
    {
      nodes[i] = malloc(sizeof(Graph));
    }
  for (size_t i = 0; i < 4; i++)
    {
      nodes[i]->tile = t;
      nodes[i]->rotate_r = nodes[(i + 1) % 4];
      nodes[i]->adjacent = NULL;
    }
  return nodes[0];
}

static void build_enforce_convexity_left (Graph *g)
{
  if (!g)
    return; /* Go away */


  Graph *clockwise = g->rotate_r->adjacent;
  Graph *ccwise = g->adjacent;

  if (!clockwise || !ccwise)
    return; /* No need to change anything */

  Graph *newclock = clockwise->rotate_r->adjacent;
  Graph *newcc = ROTATE_L(ccwise)->adjacent;

  if (!newclock)
    {
      newclock = build_initial_node();
      /* Link clockwise to newclock */
      clockwise->rotate_r->adjacent = newclock;
      newclock->adjacent = clockwise->rotate_r;
    }


  if (!newcc)
    {
      newcc = build_initial_node();
      /* Lock ccwise to newcc */
      ROTATE_L(ccwise)->adjacent = newcc;
      newcc->adjacent = ROTATE_L(ccwise);
    }

  /* Link newclock to newcc */
  newclock->rotate_r->adjacent = ROTATE_L(newcc);
  ROTATE_L(newcc)->adjacent = newclock->rotate_r;

  /* recurse counterclockwise around the perimeter of the graph */
  build_enforce_convexity_left(clockwise->rotate_r);
}

static void build_enforce_convexity_right (Graph *g)
{
  if (!g)
    return; /* Go away */


  Graph *clockwise = g->adjacent;
  Graph *ccwise = ROTATE_L(g)->adjacent;

  if (!clockwise || !ccwise)
    return; /* No need to change anything */



  /* There are THREE present nodes around this vertex, so we want to fill in
     the other two. */
  Graph *newclock = clockwise->rotate_r->adjacent;
  Graph *newcc = ROTATE_L(ccwise)->adjacent;

  if (!newclock)
    {
      newclock = build_initial_node();
      /* Link clockwise to newclock */
      clockwise->rotate_r->adjacent = newclock;
      newclock->adjacent = clockwise->rotate_r;
    }

  if (!newcc)
    {
      newcc = build_initial_node();
      /* Lock ccwise to newcc */
      ROTATE_L(ccwise)->adjacent = newcc;
      newcc->adjacent = ROTATE_L(ccwise);
    }

  /* Link newclock to newcc */
  newclock->rotate_r->adjacent = ROTATE_L(newcc);
  ROTATE_L(newcc)->adjacent = newclock->rotate_r;

  /* recurse clockwise around the perimeter of the graph */
  build_enforce_convexity_right(ROTATE_L(ccwise));
}


void build_wall_in (Graph *graph)
{
  for (size_t i = 0; i < 8; i++)
    {
      if (!graph->adjacent)
        {
          graph->adjacent = build_initial_node();
          graph->adjacent->adjacent = graph;
        }
      build_enforce_convexity_left(graph);
      build_enforce_convexity_right(graph);
      graph = graph->rotate_r;
    }
}

int build_add_node (Graph *graph, SavedTile *tile)
{
  if (!tile)
    return 0;

  char *direction_ptr = tile->path;


  if (!direction_ptr)
    return 0; /* If direction_ptr is null, we add nothing. */
  

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

          graph = ROTATE_B(graph->adjacent);
          break;
        }
    }

  graph->tile->tile_type = tile->tile_type;
  graph->tile->agent = tile->agent;

  if (tile->tile_type != TILE_TYPE_WALL)
    build_wall_in(graph);

  return 1;
}

Graph *build_graph (SavedTile *tiles)
{
  Graph *g = build_initial_node();
  build_wall_in(g);
  g->tile->tile_type = TILE_TYPE_SPACE;
  int cont = 1;
  for (size_t i = 0; cont; i++)
    {
      if (build_add_node(g, &(tiles[i])))
        continue;
      else
        break;
    }

  return g;
}


