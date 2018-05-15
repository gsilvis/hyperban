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

#ifndef __HYPERBAN_RENDERER_H
#define __HYPERBAN_RENDERER_H

#include "gui/cairo.h"

#define RENDERER_BORDER 10

enum EditAction {
  MAKE_FLOOR = 0,
  MAKE_WALL = 1,
  ROT_LEFT = 2,
  ROT_RIGHT = 3,
  MAKE_BOX = 4,
  DELETE_AGENT = 5,
  MAKE_TARGET = 6,
};

Board *js_load_board(const char *fname);
Graph* js_get_pos(Board *board);
void js_draw_graph(cairo_t *cr, Graph *graph, double width, double height, HyperbolicProjection p, Move m, double frame);
void js_dump_board(Board *board);
int js_do_move(Board *board, Move m);
Move js_undo_move(Board *board);
void js_edit_board(Board *board, enum EditAction action);
int js_get_unsolved(Board *board);
int js_get_moves(Board *board);


#endif /* __HYPERBAN_RENDERER_H */
