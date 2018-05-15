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

void js_draw_graph(cairo_t *cr, Graph *graph, double width, double height, HyperbolicProjection p, Move m, double frame) {
  renderer_draw(cr, width, height, graph, p, m, frame);
}

void js_dump_board(Board *board) {
  FILE *f = fopen("tmp_board.txt", "w");
  serialize_board(board, f);
  fclose(f);
}

int js_do_move(Board *board, Move m) {
  return perform_move(board, m);
}

Move js_undo_move(Board *board) {
  return unperform_move(board);
}


void js_edit_board(Board *board, enum EditAction action) {
  switch(action) {
  case MAKE_FLOOR:
    board->graph->adjacent->tile->tile_type = TILE_TYPE_SPACE;
    build_wall_in(board->graph->adjacent);
    return;
  case MAKE_WALL:
    board->graph->adjacent->tile->tile_type = TILE_TYPE_WALL;
    return;
  case ROT_LEFT:
    board->graph = ROTATE_L(board->graph);
    return;
  case ROT_RIGHT:
    board->graph = board->graph->rotate_r;
    return;
  case MAKE_BOX:
    if (board->graph->adjacent->tile->agent != AGENT_BOX) {
      board->graph->adjacent->tile->tile_type = TILE_TYPE_SPACE;
      build_wall_in(board->graph->adjacent);
      board->graph->adjacent->tile->agent = AGENT_BOX;
      board->unsolved++;
    }
    return;
  case DELETE_AGENT:
    if (board->graph->adjacent->tile->agent == AGENT_BOX) {
      board->unsolved--;
    }
    board->graph->adjacent->tile->agent = AGENT_NONE;
    return;
  case MAKE_TARGET:
    board->graph->adjacent->tile->tile_type = TILE_TYPE_TARGET;
    build_wall_in(board->graph->adjacent);
    return;
  }
}

int js_get_unsolved(Board *board) {
  return board->unsolved;
}

int js_get_moves(Board *board) {
  return board->number_moves;
}
