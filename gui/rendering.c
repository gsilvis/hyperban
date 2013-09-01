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

#include "rendering.h"

#include <stdlib.h>

#include "../graph/graph.h"

struct graph_queue_t {
  Graph* val;
  SquarePoints *points;
  size_t dist;
  struct graph_queue_t *next;
};

typedef struct graph_queue_t GraphQueue;

static void add_queue(GraphQueue **queue, GraphQueue **queue_end,
    Graph *graph, SquarePoints* points, size_t dist) {
  GraphQueue *n = malloc(sizeof(GraphQueue));
  n->next = NULL;
  n->val = graph;
  n->points = points;
  n->dist = dist;
  if (*queue == NULL) {
    *queue = *queue_end = n;
  } else {
    *queue_end = ((*queue_end)->next = n);
  }
}

void render_graph(RendererParams *params, Graph *graph) {
  GraphQueue *queue = malloc(sizeof(GraphQueue));
  GraphQueue *queue_end = queue;
  queue->val = graph;
  queue->next = NULL;
  queue->dist = 1;
  queue->points = malloc(sizeof(SquarePoints));
  *queue->points = *params->origin_square;

  while (queue != NULL) {
    Graph *current = queue->val;
    SquarePoints *points = queue->points;
    GraphQueue *n = queue->next;
    size_t d = queue->dist;
    free(queue);
    queue = n;

    current->tile->search_flag = 1;

    params->draw_tile(params, points, current->tile);

    if (d >= RENDERING_MAX_DIST) {
      free(points);
      continue;
    }

    if (current->adjacent && !current->adjacent->tile->search_flag) {
      add_queue(&queue, &queue_end, ROTATE_B(current->adjacent),
          move_square(points, MOVE_UP), d+1);
    }

    if (current->rotate_r->adjacent &&
        !current->rotate_r->adjacent->tile->search_flag) {
      add_queue(&queue, &queue_end, current->rotate_r->adjacent->rotate_r,
          move_square(points, MOVE_RIGHT), d+1);
    }

    if (ROTATE_B(current)->adjacent &&
        !ROTATE_B(current)->adjacent->tile->search_flag) {
      add_queue(&queue, &queue_end, ROTATE_B(current)->adjacent,
          move_square(points, MOVE_DOWN), d+1);
    }

    if (ROTATE_L(current)->adjacent &&
        !ROTATE_L(current)->adjacent->tile->search_flag) {
      add_queue(&queue, &queue_end, ROTATE_L(ROTATE_L(current)->adjacent),
          move_square(points, MOVE_LEFT), d+1);
    }
    free(points);
  }
}

SquarePoints *move_square(SquarePoints *points, Move move) {
  size_t m = move;
  SquarePoints *next_points = malloc(sizeof(SquarePoints));
  r3transform trans = hyperbolic_reflection(hyperbolic_midpoint(
      points->points[m], points->points[(m+3)%4]));
  next_points->points[m] = apply_transformation(points->points[(m+2)%4],trans);
  next_points->points[(m+1)%4] = points->points[m];
  next_points->points[(m+3)%4] = apply_transformation(points->points[(m+1)%4],
      trans);
  next_points->points[(m+2)%4] = points->points[(m+3)%4];
  return next_points;
}
