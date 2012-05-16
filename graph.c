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

#include "graph.h"

#include <stdlib.h>
#include <string.h>

void clear_dfs(Graph* graph) {
  graph->tile->dfs_use = 0;
  if (graph->adjacent && graph->adjacent->tile->dfs_use) {
    clear_dfs(graph->adjacent);
  }
  if (graph->rotate_r->adjacent && graph->rotate_r->adjacent->tile->dfs_use) {
    clear_dfs(graph->rotate_r->adjacent);
  }
  if (ROTATE_B(graph)->adjacent && ROTATE_B(graph)->adjacent->tile->dfs_use) {
    clear_dfs(ROTATE_B(graph)->adjacent);
  }
  if (ROTATE_L(graph)->adjacent && ROTATE_L(graph)->adjacent->tile->dfs_use) {
    clear_dfs(ROTATE_L(graph)->adjacent);
  }
}

void free_graph(Graph *graph) {
  Graph** all_nodes = NULL;
  size_t num_nodes = 0;
  Graph** stack = malloc(sizeof(Graph*));
  stack[0] = graph;
  size_t num_stack = 1;
  while (num_stack) {
    Graph *g = stack[--num_stack];
    if (g == NULL) continue;
    /* if g->tile is NULL, we've already added g (and it's ring) to
       all_nodes */
    if (g->tile == NULL) continue;
    Graph* ring[4] = { g, g->rotate_r, ROTATE_B(g), ROTATE_L(g)};
    free(graph->tile);
    for (int i = 0; i < 4; i++) {
      ring[i]->tile = NULL;
    }
    Graph *neighbors[4]= {
      ring[0]->adjacent,
      ring[1]->adjacent,
      ring[2]->adjacent,
      ring[3]->adjacent
    };
    stack = realloc(stack, (num_stack+4) * (sizeof(Graph*)));
    memcpy(stack+num_stack, &neighbors, 4 * sizeof(Graph*));
    num_stack += 4;

    all_nodes = realloc(all_nodes, (num_nodes+4) * sizeof(Graph*));
    memcpy(all_nodes+num_nodes, &ring, 4 * sizeof(Graph*));
    num_nodes += 4;
  }
  for (size_t i = 0; i < num_nodes; i++) {
    free(all_nodes[i]);
  }
}
