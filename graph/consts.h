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

#ifndef __HYPERBAN__CONSTS_H
#define __HYPERBAN__CONSTS_H

#include <math.h>

/* This number is the distance from the origin to the corner of a square with
   72-degree angles.  You'll probably want to divide it by sqrt(2) when using
   it for things. */
#define MAGIC 0.6871214994450249857749213334
/* sqrt(2/sqrt(5)) / sqrt(1 - 2/sqrt(5)) */

#define MORE_MAGIC (MAGIC/M_SQRT2)

#endif /* __HYPERBAN__CONSTS_H */
