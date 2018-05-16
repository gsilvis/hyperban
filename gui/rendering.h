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

#ifndef __HYPERBAN_RENDERING_H
#define __HYPERBAN_RENDERING_H

#include "../graph/types.h"
#include "matrix.h"

#define RENDERING_MAX_DIST 32

enum hyperbolic_projection_t {
  PROJECTION_KLEIN,
  PROJECTION_POINCARE,
  PROJECTION_CUSTOM
};

#define DEFAULT_PROJECTION PROJECTION_POINCARE

typedef enum hyperbolic_projection_t HyperbolicProjection;

struct renderer_params_t {
  void (*draw_tile)(struct renderer_params_t *, SquarePoints *, Tile *);
  double origin[2];
  double scale;
  HyperbolicProjection projection;
  SquarePoints *origin_square;
  void *data; // In practice this is a cairo_t*
};

typedef struct renderer_params_t RendererParams;

SquarePoints *move_square(SquarePoints *points, Move m);

void render_graph(RendererParams *params, Graph *graph);

#endif /* __HYPERBAN_RENDERING_H */

