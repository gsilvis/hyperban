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

#include "generator.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "build.h"

static const GeneratorParams default_params = {
  GENERATOR_DEFAULT_MIN_SIZE,
  GENERATOR_DEFAULT_SIZE_RANGE,
  GENERATOR_DEFAULT_MAX_DEAD_ENDS,
  GENERATOR_DEFAULT_NUM_GOALS
};

static Graph *generate_room(const GeneratorParams *params) {
  Graph *start = build_initial_node();

  size_t num_walls = 1;
  Graph **walls = malloc(sizeof(Graph *));
  walls[0] = start;
  start->tile->dfs_use = 1;

  Graph **floors = NULL;
  size_t num_floors = 0;

  size_t room_size;
  size_t goal_size = params->min_size + (rand() % params->size_range);
  goal_size += params->num_goals;
  size_t num_targets = 0;

  for (room_size = 0; room_size < goal_size; room_size++) {
    size_t index = rand() % num_walls;

    Graph *t = walls[index];

    if (rand() % (goal_size - room_size) < params->num_goals-num_targets) {
      t->tile->tile_type = TILE_TYPE_TARGET;
      num_targets++;
    } else {
      t->tile->tile_type = TILE_TYPE_SPACE;
    }

    floors = realloc(floors, ++num_floors * sizeof(Graph *));
    floors[num_floors-1] = t;

    build_wall_in(t);

    t->tile->dfs_use = 0;

    memmove(&walls[index],
        &walls[index+1], sizeof(Graph *)*(--num_walls - index));

    walls = realloc(walls, num_walls * sizeof(Graph *));

    Graph *t2 = t;
    for (size_t i = 0; i < 4; i++) {
      t2 = t2->rotate_r;
      if (t2->adjacent->tile->dfs_use == 0 &&
          t2->adjacent->tile->tile_type == TILE_TYPE_WALL) {
        walls = realloc(walls, (++num_walls) * sizeof(Graph *));
        walls[num_walls-1] = t2->adjacent;
        walls[num_walls-1]->tile->dfs_use = 1;
      }
    }
  }
  for (size_t i = 0; i < num_walls; i++) {
    walls[i]->tile->dfs_use = 0;
  }
  free(walls);

  size_t dead_ends = 0;
  for (size_t i = 0; i < num_floors; i++) {
    size_t neighbors = 0;
    Graph *t = floors[i];
    for (size_t j = 0; j < 4; j++) {
      t = t->rotate_r;
      if (t->adjacent->tile->tile_type != TILE_TYPE_WALL) neighbors++;
    }
    if (neighbors == 1) dead_ends++;
  }

  free(floors);
  if (dead_ends > params->max_dead_ends) {
    free_graph(start);
    return generate_room(params);
  } else {
   return start;
  }
}

Board *generate_board(const GeneratorParams *params) {
  srand(time(NULL));

  if (params == NULL) {
    params = &default_params;
  }

  Board *board = calloc(1, sizeof(Board));
  board->graph = generate_room(params);

  return board;
}
