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
