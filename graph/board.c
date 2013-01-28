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

#include "board.h"
#include "types.h"
#include "audit.h"
#include "build.h"
#include "graph.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RETURN_SUCCESS 1
#define RETURN_DONE -1
#define RETURN_FAILURE 0


static int board_process_config (Board *board, ConfigOption *option)
{
  if (!option)
    return RETURN_FAILURE;
  if (!(option->key))
    return RETURN_DONE;

  if (!strcmp(option->key, "startpos"))
    {
      if (!(option->value_s))
        return RETURN_FAILURE;
      if (!(board->graph))
        return RETURN_FAILURE;
      for (size_t i = 0; i < strlen(option->value_s); i++)
        {
          switch (option->value_s[i])
            {
            case 'L': case 'l':
              board->graph = board->graph->rotate_r;
              /* Fall through */
            case 'B': case 'b':
              board->graph = board->graph->rotate_r;
              /* Fall through. */
            case 'R': case 'r':
              board->graph = board->graph->rotate_r;
              break;
            case 'F': case 'f':
              if (!(board->graph->adjacent))
                return RETURN_FAILURE;
              board->graph = board->graph->adjacent;
              board->graph = ROTATE_B(board->graph->adjacent);
              break;
            default:
              fprintf(stderr, "Invalid character in string for start_loc.\n");
              return RETURN_FAILURE;
            }
        }
    }
  else if (!strcmp(option->key, "title"))
    {
      if (!(option->value_s))
        return RETURN_FAILURE;
      board->level_title = option->value_s;
    }
  else if (!strcmp(option->key, "difficulty"))
    board->difficulty = option->value_i;
  else if (!strcmp(option->key, "collection"))
    {
      if (!(option->value_s))
        return RETURN_FAILURE;
      board->collection_title = option->value_s;
    }
  else if (!strcmp(option->key, "number"))
    board->level_number = option->value_i;
  else if (!strcmp(option->key, "debug_print"))
    if (option->value_s)
      fprintf(stderr, "%s\n", option->value_s);
    else
      {
        fprintf(stderr, "No argument to debug_print\n");
        return RETURN_FAILURE;
      }
  else /* string unrecognized: return SUCCESS if not mandatory, FAILURE if
          mandatory. */
    return !(option->mandatory);

  return RETURN_SUCCESS;
}

Board *board_assemble (Graph *graph, SavedTile *tiles, ConfigOption *options)
{
  Board *board = calloc(1, sizeof(Board));
  
  board->graph = graph;
  board->unsolved = 0;

  /* Set currently run moves to 0 */
  board->number_moves = 0;
  board->moves_length = 4;
  board->moves = calloc(board->moves_length, sizeof(char));

  /* Figure out the number of unsolved boxes */
  for (int i = 0; tiles[i].path; i++)
    if ((tiles[i].agent == AGENT_BOX) && (tiles[i].tile_type != TILE_TYPE_TARGET))
      (board->unsolved)++;

  int cont = 1;
  /* Process the ConfigOptions */
  for (int i = 0; cont; i++)
    {
      switch (board_process_config(board, &(options[i])))
        {
        case RETURN_FAILURE:
        default:
          fprintf(stderr, "Adding config option failed.\n");
          return NULL;
        case RETURN_SUCCESS:
          break; /* continue iterating */
        case RETURN_DONE:
          cont = 0;
          break;
        }
    }

  return board;
}

Board *board_assemble_full (SavedTile *tiles, ConfigOption *options)
{
  if (!audit_level(tiles, options))
    return NULL;
  Graph *graph = build_graph(tiles);
  Board *board = board_assemble(graph, tiles, options);
  if (!audit_board(board))
    return NULL;

  return board;
}

void free_board (Board *b)
{
  if (b->graph)
    free_graph(b->graph);
  if (b->moves)
    free(b->moves);
  if (b->level_title)
    free(b->level_title);
  if (b->collection_title)
    free(b->collection_title);
  free(b);
}
