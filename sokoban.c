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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char sokoban_get_move_abbreviation (Move move, int is_push)
{
  char returnee = 32 * (1 - is_push); 
  /* If it's a push, we'll make it uppercase */
  switch (move)
    {
    case MOVE_UP:
      return returnee + 'U';
    case MOVE_RIGHT:
      return returnee + 'R';
    case MOVE_DOWN:
      return returnee + 'D';
    case MOVE_LEFT:
      return returnee + 'L';
    default:
      return '?';
    }
}

int sokoban_update_board_data (Board *b, Move move, int is_push)
{
  b->number_moves++;
  if (!(b->moves))
    return is_push; /* and don't do anything */

  char c = sokoban_get_move_abbreviation(move, is_push);

  if (strlen(b->moves) >= b->moves_length - 1)
    {
      b->moves_length *= 2;
      b->moves = realloc(b->moves, b->moves_length);
    }
  strncat(b->moves, &c, 1); /* add a letter to the end */

  return is_push;
}

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
      return sokoban_update_board_data(b, move, RESULT_MOVE);
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
  return sokoban_update_board_data(b, move, RESULT_PUSH); /* Push sucessful */
}



char unperform_move (Board *b)
{
  if (!(b->moves))
    return '\0'; /* Undoing not set up correctly */
  if (strlen(b->moves) == 0)
    return '\0'; /* No moves have been made */
  char c = b->moves[strlen(b->moves) - 1];
  b->moves[strlen(b->moves) - 1] = '\0';
  Graph *bluh = b->graph;
  switch (c)
    {
    case 'L': case 'l':
      bluh = bluh->rotate_r;
    case 'D': case 'd':
      bluh = bluh->rotate_r;
    case 'R': case 'r':
      bluh = bluh->rotate_r;
    case 'U': case 'u':
      break;
    default:
      return '\0'; /* illegal character */
    }

  /* First we check that we can walk backwards */
  if (!(bluh->rotate_r->rotate_r->adjacent))
    return '\0'; /* we can't */


  if (c < 'a') /* then it's uppercase */
    {
      if (bluh->adjacent->tile->agent != AGENT_BOX)
        return '\0'; /* There's no box... */

      bluh->adjacent->tile->agent = AGENT_NONE;
      bluh->tile->agent = AGENT_BOX; /* pull box */
    }
  
  /* now step backwards */
  b->graph = bluh->rotate_r->rotate_r->adjacent;

  /* and spin until we're facing the right way */
  /* note that the cases are in the OPPOSITE direction to as above! */
  switch (c)
    {
    case 'R': case 'r':
      b->graph = b->graph->rotate_r;
    case 'D': case 'd':
      b->graph = b->graph->rotate_r;
    case 'L': case 'l':
      b->graph = b->graph->rotate_r;
    case 'U': case 'u':
      break;
    }


  /* one less move has been done */
  (b->number_moves)--;

  return c;
}
