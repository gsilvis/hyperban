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

#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#include "cairo.h"

#include "./widgets.h"
#include "../renderer.h"
#include "../graph/graph.h"
#include "./matrix.h"
#include "../graph/level.h"
#include "../graph/sokoban.h"
#include "../graph/board.h"
#include "./rendering.h"
#include "./cairo_helper.h"
#include "../graph/generator.h"
#include "../graph/serialize.h"

void draw_tile(RendererParams *params, SquarePoints *points,
    Tile *tile) {
  cairo_t *cr = params->data;
  if (params->projection == PROJECTION_KLEIN) {
    for (size_t i = 0; i < 4; i++) {
      r3vector projected = points->points[i];
      cairo_line_to(cr, projected.els[0], projected.els[1]);
    }
    cairo_close_path(cr);
  } else if (params->projection == PROJECTION_POINCARE) {
    r3vector projected[4] = {
      klein2poincare(points->points[0]),
      klein2poincare(points->points[1]),
      klein2poincare(points->points[2]),
      klein2poincare(points->points[3])
    };
    r3vector midpoints[4] = {
      klein2poincare(hyperbolic_midpoint(points->points[0], points->points[1])),
      klein2poincare(hyperbolic_midpoint(points->points[1], points->points[2])),
      klein2poincare(hyperbolic_midpoint(points->points[2], points->points[3])),
      klein2poincare(hyperbolic_midpoint(points->points[3], points->points[0]))
    };
    cairo_move_to(cr, projected[0].els[0], projected[0].els[1]);
    for (size_t i = 0; i < 4; i++) {
      circle_to(cr, projected[i].els[0], projected[i].els[1], midpoints[i].els[0],
          midpoints[i].els[1], projected[(i+1)%4].els[0], projected[(i+1)%4].els[1]);
    }
    cairo_close_path(cr);
  }
  if (tile->tile_type == TILE_TYPE_SPACE) {
    if (tile->agent == AGENT_BOX)
      cairo_set_source_rgb(cr, 0, 0, 1);
    else
      cairo_set_source_rgb(cr, 1, 1, 1);
  } else if (tile->tile_type == TILE_TYPE_TARGET) {
    if (tile->agent == AGENT_BOX)
      cairo_set_source_rgb(cr, 0, 1, 0);
    else
      cairo_set_source_rgb(cr, 1, 1, 0);
  } else if (tile->tile_type == TILE_TYPE_WALL) {
    cairo_set_source_rgb(cr, .25, .25, .25);
  }

  cairo_fill_preserve(cr);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_stroke(cr);
}

void renderer_draw(cairo_t *cr, double width, double height,
    Graph* graph, HyperbolicProjection projection, Move m, double frame) {
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);

  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_width(cr, 2);

  double radius = fmin(width, height)/2 - RENDERER_BORDER;

  double originx = width / 2;
  double originy = height / 2;

  cairo_move_to(cr, originx + radius, originy);

  cairo_arc(cr, originx, originy, radius,
      0, 2 * M_PI);

  cairo_set_source_rgb(cr, .5, .5, .5);
  cairo_fill_preserve(cr);

  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_stroke(cr);

  clear_search(graph);

  SquarePoints *origin = new_squarepoints();
  *origin = origin_square;

  if (frame != 0) {
    SquarePoints* next = move_square(origin, (m + 2) % 4);
    r3vector from = {{0, 0, 1}};
    r3vector to = hyperbolic_midpoint(
        hyperbolic_midpoint(next->points[0], next->points[1]),
        hyperbolic_midpoint(next->points[2], next->points[3]));
    to.els[1] *= frame;
    to.els[0] *= frame;
    to.els[2] = 1;
    r3transform transformation;
    hyperbolic_translation(from, to, &transformation);
    SquarePoints *temp = transform_square(origin, &transformation);
    free(origin);
    free(next);
    origin = temp;
  }

  RendererParams params = {
    draw_tile,
    {originx, originy},
    radius,
    projection,
    origin,
    cr
  };

  cairo_set_line_width(cr, 1/params.scale);

  cairo_translate(cr, params.origin[0], params.origin[1]);
  cairo_scale(cr, params.scale, params.scale);

  render_graph(&params, graph);

  free(origin);

  return;
}

