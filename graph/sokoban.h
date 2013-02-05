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

#ifndef __HYPERBAN__SOKOBAN_H
#define __HYPERBAN__SOKOBAN_H

#include <stddef.h>

#include "types.h"
#include "graph.h"

typedef enum {
  RESULT_NO_MOVE_POSSIBLE = -1,
  RESULT_MOVE = 0,
  RESULT_PUSH = 1,
} move_result_t;

move_result_t perform_move (Board *g, Move move);
char unperform_move (Board *b);

#endif /* __HYPERBAN__SOKOBAN_H */
