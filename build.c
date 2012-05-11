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

Graph *build_initial_node ()
{
  Graph (*first)[4] = malloc(4 * sizeof(Graph));
  for (int i = 0; i < 4; i++)
    first[i]->rotate_r = first[(i+1) % 4];
  Tile *t = malloc(sizeof(Tile));

  /* Set the Tile to the DEFAULT VALUES */
  t->tile_type = TILE_TYPE_DEFAULT;
  t->agent = AGENT_DEFAULT;
  t->dfs_use = 0;
  
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

  graph->tile->tile_type = tile->tile_type;
  graph->tile->agent = tile->agent;



}



Graph *build_graph (SavedTile *tiles, size_t num_tiles)
{
  Graph *g = build_initial_node();
  for (size_t i = 0; i < num_tiles; i++)
    build_add_node(g, &tiles[i]);
  return g;
}


