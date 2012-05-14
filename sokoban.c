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

#include "sokoban.h"
#include "types.h"
#include "graph.h"

int perform_move (Board *b, Move move)
{
  Graph *old = b->graph;
  Graph *new = old;
  for (char i = 0; i < move; i++)
    new = new->rotate_r;
  new = new->adjacent;
  // Re-align orientation
  for (char i = 0; i < (2 - move + 4) % 4; i++)
    new = new->rotate_r;

  if (new->tile->tile_type == TILE_TYPE_WALL)
    return RESULT_NO_MOVE_POSSIBLE; /* No move allowed */

  if (new->tile->agent == AGENT_NONE)
    {
      b->graph = new; /* Walk */
      return RESULT_MOVE;
    }


  /* There's a box, so we need to figure out what's behind it */

  Graph *behind = new;
  for (char i = 0; i < move; i++)
    behind = behind->rotate_r;
  behind = behind->adjacent;

  if (behind->tile->tile_type == TILE_TYPE_WALL)
    return RESULT_NO_MOVE_POSSIBLE; /* Push blocked by wall. */

  if (behind->tile->tile_type == AGENT_BOX)
    return RESULT_NO_MOVE_POSSIBLE; /* Push blocked by box. */

  /* There's nothing behind the box, so the box moves */

  new->tile->agent = AGENT_NONE;
  behind->tile->agent = AGENT_BOX;

  if (new->tile->tile_type == TILE_TYPE_TARGET)
    (b->unsolved)++;

  if (behind->tile->tile_type == TILE_TYPE_TARGET)
    (b->unsolved)--;

  b->graph = new;
  return RESULT_PUSH; /* Push sucessful */
}
