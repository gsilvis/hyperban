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
#include <string.h>
#include <stdio.h>

#include "gui/widgets.h"
#include "renderer.h"
#include "graph/graph.h"
#include "gui/matrix.h"
#include "graph/level.h"
#include "graph/sokoban.h"
#include "graph/board.h"
#include "graph/generator.h"
#include "graph/serialize.h"

Board *js_load_board(const char *fname) {
  FILE *in = fopen(fname, "r");
  if (in == NULL) return NULL;

  SavedTile *tiles;
  ConfigOption *options;
  if (level_parse_file(in, &tiles, &options)) return NULL;

  fclose(in);

  return board_assemble_full(tiles, options);
}

Graph* js_get_pos(Board *board) {
  return board->graph;
}

void js_draw_graph(Graph *graph, double width, double height, HyperbolicProjection p, Move m, double frame) {
  renderer_draw(NULL, width, height, graph, p, m, frame);
}

void js_dump_board(Board *board) {
  FILE *f = fopen("tmp_board.txt", "w");
  serialize_board(board, f);
  fclose(f);
}

int js_do_move(Board *board, Move m) {
  return perform_move(board, m);
}
