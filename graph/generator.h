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

#ifndef __HYPERBAN_GENERATOR_H
#define __HYPERBAN_GENERATOR_H

#include "types.h"

struct generator_params_t {
  size_t min_size;
  size_t size_range;
  size_t max_dead_ends;
  size_t num_goals;
};

typedef struct generator_params_t GeneratorParams;

Board *generate_board(const GeneratorParams *params);

#define GENERATOR_DEFAULT_MIN_SIZE 10
#define GENERATOR_DEFAULT_SIZE_RANGE 5
#define GENERATOR_DEFAULT_MAX_DEAD_ENDS 0
#define GENERATOR_DEFAULT_NUM_GOALS 2

#endif /* __HYPERBAN_GENERATOR_H */
