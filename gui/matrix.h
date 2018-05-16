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

#ifndef __HYPERBAN_MATRIX_H
#define __HYPERBAN_MATRIX_H

#include <math.h>
#include <stddef.h>

typedef double matrix_el_t;

typedef struct { matrix_el_t els[3]; } r3vector;

typedef struct { matrix_el_t els[9]; } r3transform;

typedef struct square_points_t {
  r3vector points[4];
} SquarePoints;

static inline r3vector const_r3vector(matrix_el_t a) {
  return (r3vector) {{
    a,a,a
  }};
}
static inline r3transform const_r3transform(matrix_el_t a) {
  return (r3transform) {{
    a,a,a,
    a,a,a,
    a,a,a
  }};
}

SquarePoints *new_squarepoints(void);
void hyperbolic_reflection(r3vector a, r3transform *out);
r3vector hyperbolic_midpoint(r3vector a, r3vector b);
r3vector apply_transformation(r3vector a, r3transform *b);
void hyperbolic_translation(r3vector a, r3vector b, r3transform *out);

r3vector klein2poincare(r3vector a);
r3vector klein2custom(r3vector a);

extern const SquarePoints origin_square;
SquarePoints *transform_square(SquarePoints *points, r3transform *trans);

extern double CUSTOM_PROJECTION_PARAM;

#endif /* __HYPERBAN_MATRIX_H */
