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

#include "consts.h"
#include "graph.h"

struct graph_queue_t {
  Graph* val;
  SquarePoints *points;
  size_t dist;
  struct graph_queue_t *next;
};

typedef struct graph_queue_t GraphQueue;

/* returns points in clockwise order */
SquarePoints *get_origin_square(void) {
  SquarePoints *res = malloc(sizeof(SquarePoints));
  *res = (SquarePoints) {
    {
      { MORE_MAGIC, -MORE_MAGIC, 0, 1},
      { MORE_MAGIC,  MORE_MAGIC, 0, 1},
      {-MORE_MAGIC,  MORE_MAGIC, 0, 1},
      {-MORE_MAGIC, -MORE_MAGIC, 0, 1}
    }
  };
  return res;
}

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
  queue->points = get_origin_square();

  while (queue != NULL) {
    Graph *current = queue->val;
    SquarePoints *points = queue->points;
    GraphQueue *n = queue->next;
    size_t d = queue->dist;
    free(queue);
    queue = n;

    current->tile->dfs_use = 1;

    params->draw_tile(params, points, current->tile);

    if (d >= RENDERING_MAX_DIST) {
      free(points);
      continue;
    }

    if (current->adjacent && !current->adjacent->tile->dfs_use) {
      r4transform trans =
          hyperbolic_reflection(hyperbolic_midpoint(points->points[0],
              points->points[3]));
      SquarePoints *next_points = malloc(sizeof(SquarePoints));
      next_points->points[0] = apply_transformation(points->points[2], trans);
      next_points->points[1] = points->points[0];
      next_points->points[2] = points->points[3];
      next_points->points[3] = apply_transformation(points->points[1], trans);
      add_queue(&queue, &queue_end, ROTATE_B(current->adjacent), next_points,
          d+1);
    }

    if (current->rotate_r->adjacent &&
        !current->rotate_r->adjacent->tile->dfs_use) {
      r4transform trans =
          hyperbolic_reflection(hyperbolic_midpoint(points->points[0],
              points->points[1]));
      SquarePoints *next_points = malloc(sizeof(SquarePoints));
      next_points->points[0] = apply_transformation(points->points[2], trans);
      next_points->points[1] = apply_transformation(points->points[3], trans);
      next_points->points[2] = points->points[1];
      next_points->points[3] = points->points[0];
      add_queue(&queue, &queue_end, current->rotate_r->adjacent->rotate_r,
        next_points, d+1);
    }

    if (ROTATE_B(current)->adjacent &&
        !ROTATE_B(current)->adjacent->tile->dfs_use) {
      r4transform trans =
          hyperbolic_reflection(hyperbolic_midpoint(points->points[1],
              points->points[2]));
      SquarePoints *next_points = malloc(sizeof(SquarePoints));
      next_points->points[0] = points->points[1];
      next_points->points[1] = apply_transformation(points->points[3], trans);
      next_points->points[2] = apply_transformation(points->points[0], trans);
      next_points->points[3] = points->points[2];
      add_queue(&queue, &queue_end, ROTATE_B(current)->adjacent,
        next_points, d+1);
    }

    if (ROTATE_L(current)->adjacent &&
        !ROTATE_L(current)->adjacent->tile->dfs_use) {
      r4transform trans =
          hyperbolic_reflection(hyperbolic_midpoint(points->points[2],
              points->points[3]));
      SquarePoints *next_points = malloc(sizeof(SquarePoints));
      next_points->points[0] = points->points[3];
      next_points->points[1] = points->points[2];
      next_points->points[2] = apply_transformation(points->points[0], trans);
      next_points->points[3] = apply_transformation(points->points[1], trans);
      add_queue(&queue, &queue_end, ROTATE_L(ROTATE_L(current)->adjacent),
          next_points, d+1);
    }
    free(points);
  }
}
