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

typedef matrix_el_t
    r3vector __attribute__ ((vector_size(sizeof(matrix_el_t)*4)));

typedef matrix_el_t
    r3transform __attribute__((vector_size(sizeof(matrix_el_t)*4*4)));

typedef struct square_points_t {
  r3vector points[4];
} SquarePoints;

static inline r3vector const_r3vector(matrix_el_t a) {
  return (r3vector) {
      a,a,a
    };
}
static inline r3transform const_r3transform(matrix_el_t a) {
  return (r3transform) {
      a,a,a,
      a,a,a,
      a,a,a
    };
}

matrix_el_t minkowski_self_inner_product(r3vector a);

matrix_el_t minkowski_inner_product(r3vector a, r3vector b);

r3transform outer_product(r3vector a, r3vector b);

r3vector normalize_r3vector(r3vector a);

r3vector apply_transformation(r3vector a, r3transform b);

r3transform multiply_transformations(r3transform a, r3transform b);

matrix_el_t hyperbolic_distance(r3vector a, r3vector b);

static inline r3transform identity_transform(void) {
  return (r3transform) {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
  };
}

static inline r3transform hyperbolic_identity_transform(void) {
  return (r3transform) {
    1, 0, 0,
    0, 1, 0,
    0, 0, -1,
  };
}

r3transform hyperbolic_reflection(r3vector a);

r3vector hyperbolic_midpoint(r3vector a, r3vector b);

r3transform hyperbolic_translation(r3vector a, r3vector b);

r3vector weierstrass2poincare(r3vector a);

r3vector poincare2weierstrass(r3vector a);

r3vector weierstrass2klein(r3vector a);

r3vector klein2weierstrass(r3vector a);

r3vector klein2poincare(r3vector a);

r3vector poincare2klein(r3vector a);

SquarePoints *transform_square(SquarePoints *points, r3transform *trans);

SquarePoints *get_origin_square(void);

#endif /* __HYPERBAN_MATRIX_H */
