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

// n = rows in mat1/res, m = cols in mat1/rows in mat2, l = cols in mat2/res

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

static inline r3vector const_r3vector(matrix_el_t a) {
  return (r3vector) { a, a, a };
}

static inline r3transform const_r3transform(matrix_el_t a) {
  return (r3transform) {
    a, a, a,
    a, a, a,
    a, a, a,
  };
}

static inline matrix_el_t minkowski_self_inner_product(r3vector a) {
  return a[0] * a[0] + a[1] * a[1] - a[2] * a[2];
}

static inline matrix_el_t minkowski_inner_product(r3vector a, r3vector b) {
  return a[0] * b[0] + a[1] * b[1] - a[2] * b[2];
}

static inline r3transform outer_product(r3vector a, r3vector b) {
  r3transform result;
  _MAT_MULT(a, b, 3, 1, 3, result);
  return result;
}

static inline r3vector normalize_r3vector(r3vector a) {
  return a / const_r3vector(a[2]);
}

static inline r3vector apply_transformation(r3vector a, r3transform b) {
  r3vector result;
  _MAT_MULT(b, a, 3, 3, 1, result);
  return normalize_r3vector(result);
};

static inline r3transform multiply_transformations(r3transform a,
    r3transform b) {
  r3transform result;
  _MAT_MULT(a, b, 3, 3, 3, result);
  return result;
}

static inline matrix_el_t hyperbolic_distance(r3vector a, r3vector b) {
  matrix_el_t numerator = minkowski_inner_product(a, b);
  numerator *= numerator;

  matrix_el_t denominator = minkowski_self_inner_product(a);
  denominator *= minkowski_self_inner_product(b);

  return 2 * acosh(sqrt(numerator/denominator));
}

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

static inline r3transform hyperbolic_reflection(r3vector a) {
  matrix_el_t denom = minkowski_self_inner_product(a);

  r3vector swapped = a;
  swapped[2] *= -1;

  swapped *= const_r3vector(2.0/denom);

  return identity_transform() - outer_product(a, swapped);
}

static inline r3vector hyperbolic_midpoint(r3vector a, r3vector b) {
  matrix_el_t t1 = minkowski_self_inner_product(a);
  matrix_el_t t2 = minkowski_self_inner_product(b);
  matrix_el_t t3 = minkowski_inner_product(a, b);

  matrix_el_t c1 = sqrt(t2 * t3);
  matrix_el_t c2 = sqrt(t1 * t3);

  return normalize_r3vector(a * const_r3vector(c1) + b * const_r3vector(c2));
}

static inline r3transform hyperbolic_translation(r3vector a, r3vector b) {
  r3transform rm = hyperbolic_reflection(hyperbolic_midpoint(a, b));
  r3transform ra = hyperbolic_reflection(a);

  return multiply_transformations(rm, ra);
}

static inline r3vector weierstrass2poincare(r3vector a) {
  return a / const_r3vector((a[2] + 1));
}

static inline r3vector poincare2weierstrass(r3vector a) {
  matrix_el_t d = 1.0 / (1 - a[0]*a[0] - a[1]*a[1]);
  r3vector result = {2*a[0], 2*a[1], (1 + a[0] * a[0] + a[1] * a[1])};
  return result * const_r3vector(d);
}

static inline r3vector weierstrass2klein(r3vector a) {
  return normalize_r3vector(a);
}

static inline r3vector klein2weierstrass(r3vector a) {
  matrix_el_t d = sqrt(1 - a[0]*a[0] - a[1]*a[1]);
  return a / const_r3vector(d);
}

static inline r3vector klein2poincare(r3vector a) {
  matrix_el_t d = sqrt(1 - a[0]*a[0] - a[1]*a[1]) + 1.0;
  return a / const_r3vector(d);
}

static inline r3vector poincare2klein(r3vector a) {
  return weierstrass2klein(poincare2weierstrass(a));
}

#endif /* __HYPERBAN_MATRIX_H */
