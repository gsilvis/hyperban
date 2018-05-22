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

#ifndef __HYPERBAN__TYPES_H
#define __HYPERBAN__TYPES_H

#include <stddef.h>

typedef struct {
  char tile_type;
  char agent;
  char search_flag;
} Tile;

enum {
  TILE_TYPE_SPACE = 0,
  TILE_TYPE_WALL = 1,
  TILE_TYPE_TARGET = 2,
  TILE_TYPE_DEFAULT = TILE_TYPE_WALL,
};

enum {
  AGENT_NONE = 0,
  AGENT_BOX = 1,
  AGENT_DEFAULT = AGENT_NONE,
};

typedef char Move;

enum {
  MOVE_UP = 0,
  MOVE_RIGHT = 1,
  MOVE_DOWN = 2,
  MOVE_LEFT = 3,
};

typedef struct {
  char *path;
  char tile_type;
  char agent;
} SavedTile;

typedef struct {
  char *key;
  char mandatory;
  char *value_s;
  int value_i;
} ConfigOption;


typedef struct graph_t {
  struct graph_t *rotate_r;
  struct graph_t *adjacent;
  Tile *tile;
} Graph;

typedef struct {
  Graph* graph;
  int unsolved;
  int difficulty;
  int level_number;
  char *level_title;
  char *collection_title;
  char *moves;
  size_t moves_length;
  int number_moves;
  char *filename;
  /* expand as needed */
} Board;

#endif /* __HYBERBAN__TYPES_H */
