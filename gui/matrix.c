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

#include "matrix.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../graph/consts.h"

static matrix_el_t minkowski_self_inner_product(r3vector a);

static matrix_el_t minkowski_inner_product(r3vector a, r3vector b);

static void outer_product(r3vector a, r3vector b, r3transform *out);

static r3vector normalize_r3vector(r3vector a);

static void multiply_transformations(r3transform *a, r3transform *b, r3transform *out);

static matrix_el_t hyperbolic_distance(r3vector a, r3vector b);

static const r3transform identity_transform;
static const r3transform hyperbolic_identity_transform;

static r3vector weierstrass2poincare(r3vector a);

static r3vector poincare2weierstrass(r3vector a);

static r3vector weierstrass2klein(r3vector a);

static r3vector klein2weierstrass(r3vector a);

static r3vector poincare2klein(r3vector a);

// n = rows in mat1/res, m = cols in mat1/rows in mat2, l = cols in mat2/res
// This is a macro because we use it on both r3vectors and r3transforms

#define _MAT_MULT(mat1, mat2, n, m, l, res) \
  do { \
    for (size_t _n = 0; _n < (n); _n++) { \
      for (size_t _l = 0; _l < (l); _l++) { \
        (res)[(l)*_n+_l] = 0; \
        for (size_t _m = 0; _m < (m); _m++) { \
          (res)[(l) * _n + _l] += (mat1)[_n*(m)+_m] * (mat2)[_m*(l)+_l]; \
        } \
      } \
    } \
  } while (0)

matrix_el_t minkowski_self_inner_product(r3vector a) {
  return a[0] * a[0] + a[1] * a[1] - a[2] * a[2];
}

matrix_el_t minkowski_inner_product(r3vector a, r3vector b) {
  return a[0] * b[0] + a[1] * b[1] - a[2] * b[2];
}

void outer_product(r3vector a, r3vector b, r3transform *out) {
  _MAT_MULT(a, b, 3, 1, 3, *out);
}

r3vector normalize_r3vector(r3vector a) {
  return a / const_r3vector(a[2]);
}

r3vector apply_transformation(r3vector a, r3transform *b) {
  r3vector result;
  _MAT_MULT(*b, a, 3, 3, 1, result);
  return normalize_r3vector(result);
};

void multiply_transformations(r3transform *a, r3transform *b, r3transform *o) {
  _MAT_MULT(*a, *b, 3, 3, 3, *o);
}

matrix_el_t hyperbolic_distance(r3vector a, r3vector b) {
  matrix_el_t numerator = minkowski_inner_product(a, b);
  numerator *= numerator;

  matrix_el_t denominator = minkowski_self_inner_product(a);
  denominator *= minkowski_self_inner_product(b);

  return 2 * acosh(sqrt(numerator/denominator));
}

void hyperbolic_reflection(r3vector a, r3transform *out) {
  matrix_el_t denom = minkowski_self_inner_product(a);

  matrix_el_t s = 2.0 / denom;

  r3vector swapped = {a[0]*s, a[1]*s, a[2]*-1*s};

  outer_product(a, swapped, out);

  *out *= -1;
  *out += identity_transform;
}

r3vector hyperbolic_midpoint(r3vector a, r3vector b) {
  matrix_el_t t1 = minkowski_self_inner_product(a);
  matrix_el_t t2 = minkowski_self_inner_product(b);
  matrix_el_t t3 = minkowski_inner_product(a, b);

  matrix_el_t c1 = sqrt(t2 * t3);
  matrix_el_t c2 = sqrt(t1 * t3);

  return normalize_r3vector(a * const_r3vector(c1) + b * const_r3vector(c2));
}

void hyperbolic_translation(r3vector a, r3vector b, r3transform *o) {
  r3transform rm;
  hyperbolic_reflection(hyperbolic_midpoint(a, b), &rm);

  r3transform ra;
  hyperbolic_reflection(a, &ra);

  multiply_transformations(&rm, &ra, o);
}

r3vector weierstrass2poincare(r3vector a) {
  return a / const_r3vector((a[2] + 1));
}

r3vector poincare2weierstrass(r3vector a) {
  matrix_el_t d = 1.0 / (1 - a[0]*a[0] - a[1]*a[1]);
  r3vector result = {2*a[0], 2*a[1], (1 + a[0] * a[0] + a[1] * a[1])};
  return result * const_r3vector(d);
}

r3vector weierstrass2klein(r3vector a) {
  return normalize_r3vector(a);
}

r3vector klein2weierstrass(r3vector a) {
  matrix_el_t d = sqrt(1 - a[0]*a[0] - a[1]*a[1]);
  return a / const_r3vector(d);
}

r3vector klein2poincare(r3vector a) {
  matrix_el_t d = sqrt(1 - a[0]*a[0] - a[1]*a[1]) + 1.0;
  return a / const_r3vector(d);
}

r3vector poincare2klein(r3vector a) {
  return weierstrass2klein(poincare2weierstrass(a));
}

SquarePoints *transform_square(SquarePoints *square, r3transform *trans) {
  SquarePoints *result = new_squarepoints();
  *result = (SquarePoints) {
    {
      apply_transformation(square->points[0], trans),
      apply_transformation(square->points[1], trans),
      apply_transformation(square->points[2], trans),
      apply_transformation(square->points[3], trans)
    }
  };
  return result;
}

const SquarePoints origin_square = {
  {
    { MORE_MAGIC, -MORE_MAGIC, 1},
    { MORE_MAGIC,  MORE_MAGIC, 1},
    {-MORE_MAGIC,  MORE_MAGIC, 1},
    {-MORE_MAGIC, -MORE_MAGIC, 1}
  }
};

SquarePoints *new_squarepoints(void) {
  void *f = NULL;

  posix_memalign(&f, 0x20, sizeof(SquarePoints));

  return (SquarePoints*)f;
}

static const r3transform identity_transform = {
  1, 0, 0,
  0, 1, 0,
  0, 0, 1,
};

static const r3transform hyperbolic_identity_transform = {
  1, 0, 0,
  0, 1, 0,
  0, 0, -1,
};
