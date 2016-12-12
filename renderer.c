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

#include "gui/widgets.h"
#include "renderer.h"
#include "graph/graph.h"
#include "gui/matrix.h"
#include "graph/level.h"
#include "graph/sokoban.h"
#include "graph/board.h"
#include "graph/generator.h"

static Board *theboard;

int load_board(const char *fname) {
  FILE *in = fopen("levels/easy.txt", "r");
  if (in == NULL) return -1;

  SavedTile *tiles;
  ConfigOption *options;
  if (level_parse_file(in, &tiles, &options)) return -1;

  fclose(in);

  theboard = board_assemble_full(tiles, options);

  return 0;
}

void draw_board(cairo_t *cr, double width, double height) {
  renderer_draw(cr, width, height, theboard->graph, DEFAULT_PROJECTION, 0, 0);
}


int do_keypress(int keyCode) {
  Move m = -1;

  switch(keyCode) {
  case KEY_UP:
    m = MOVE_UP;
    break;
  case KEY_DOWN:
    m = MOVE_DOWN;
    break;
  case KEY_LEFT:
    m = MOVE_LEFT;
    break;
  case KEY_RIGHT:
    m = MOVE_RIGHT;
    break;
/*
  case KEY_UNDO:
    if (!opts->editing)
      m = -2;
    break;
  case KEY_MAKE_FLOOR:
    if (opts->editing) {
      opts->board->graph->adjacent->tile->tile_type = TILE_TYPE_SPACE;
      build_wall_in(opts->board->graph->adjacent);
    }
    break;
  case KEY_MAKE_WALL:
    if (opts->editing) {
      opts->board->graph->adjacent->tile->tile_type = TILE_TYPE_WALL;
    }
    break;
  case KEY_ROT_LEFT:
    if (opts->editing)
      opts->board->graph = ROTATE_L(opts->board->graph);
    break;
  case KEY_ROT_RIGHT:
    if (opts->editing)
      opts->board->graph = opts->board->graph->rotate_r;
    break;
  case KEY_MAKE_BOX:
    if (opts->editing) {
      if (opts->board->graph->adjacent->tile->agent != AGENT_BOX) {
        opts->board->graph->adjacent->tile->tile_type = TILE_TYPE_SPACE;
        build_wall_in(opts->board->graph->adjacent);
        opts->board->graph->adjacent->tile->agent = AGENT_BOX;
        opts->board->unsolved++;
      }
    }
    break;
  case KEY_DELETE_AGENT:
    if (opts->editing) {
      if (opts->board->graph->adjacent->tile->agent == AGENT_BOX) {
        opts->board->unsolved--;
      }
      opts->board->graph->adjacent->tile->agent = AGENT_NONE;
    }
    break;
  case KEY_MAKE_TARGET:
    if (opts->editing) {
      opts->board->graph->adjacent->tile->tile_type = TILE_TYPE_TARGET;
      build_wall_in(opts->board->graph->adjacent);
    }
    break;
  case KEY_SAVE:
    if (opts->editing) {
      FILE *f = fopen(opts->board->filename, "w");
      serialize_board(opts->board, f);
      fclose(f);
    }
    break;
  case KEY_HELP:
    toggle_help(opts);
    return FALSE;
*/
  default:
    return 0;
  }


  perform_move(theboard, m);
  return 1;
}
