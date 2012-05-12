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

typedef float matrix_el_t;

typedef matrix_el_t r4vector __attribute__ ((vector_size(sizeof(float)*4)));

typedef matrix_el_t r4transform __attribute__((vector_size(sizeof(float)*4*4)));

// n = rows in mat1/res, m = cols in mat1/rows in mat2, l = cols in mat2/res

#define _MAT_MULT(mat1, mat2, n, m, l, res) \
  do { \
    for (size_t _n = 0; _n < n; _n++) { \
      for (size_t _l = 0; _l < l; _l++) { \
        (res)[l * _n + _l] = 0; \
        for (size_t _m = 0; _m < m; _m++) { \
          (res)[l * _n + _l] += (mat1)[_n * m + _m] * (mat2)[_m * l + _l]; \
        } \
      } \
    } \
  } while (0)

inline r4vector const_r4vector(matrix_el_t a) {
  r4vector res = { a, a, a, a };
  return res;
}

inline r4transform const_r4transform(matrix_el_t a) {
  r4transform res = {
    a, a, a, a,
    a, a, a, a,
    a, a, a, a,
    a, a, a, a
  };
  return res;
}

inline matrix_el_t minkowski_inner_product(r4vector a, r4vector b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] - a[3] * b[3];
}

inline r4transform outer_product(r4vector a, r4vector b) {
  r4transform result;
  _MAT_MULT(a, b, 4, 1, 4, result);
  return result;
}

inline r4vector normalize_r4vector(r4vector a) {
  r4vector result = { a[0]/a[3], a[1]/a[3], a[2]/a[3], 1 };
  return result;
}

inline r4vector apply_transformation(r4vector a, r4transform b) {
  r4vector result;
  _MAT_MULT(b, a, 4, 4, 1, result);
  return normalize_r4vector(result);
};

inline r4transform multiply_transformations(r4transform a, r4transform b) {
  r4transform result;
  _MAT_MULT(a, b, 4, 4, 4, result);
  return result;
}

inline matrix_el_t hyperbolic_distance(r4vector a, r4vector b) {
  matrix_el_t numerator = minkowski_inner_product(a, b);
  numerator *= numerator;

  matrix_el_t denominator = minkowski_inner_product(a, a);
  denominator *= minkowski_inner_product(b, b);

  return 2 * acosh(sqrt(numerator/denominator));
}

inline r4transform identity_transform(void) {
  r4transform result = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };
  return result;
}

inline r4transform hyperbolic_identity_transform(void) {
  r4transform result = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, -1
  };
  return result;
}

inline r4transform hyperbolic_reflection(r4vector a) {
  matrix_el_t denom = minkowski_inner_product(a, a);

  r4transform result = multiply_transformations(outer_product(a, a),
      hyperbolic_identity_transform());

  return identity_transform() - (result * const_r4transform(2.0/denom));
}

inline r4vector hyperbolic_midpoint(r4vector a, r4vector b) {
  matrix_el_t t1 = minkowski_inner_product(a, a);
  matrix_el_t t2 = minkowski_inner_product(b, b);
  matrix_el_t t3 = minkowski_inner_product(a, b);

  matrix_el_t c1 = sqrt(t2 * t3);
  matrix_el_t c2 = sqrt(t1 * t3);

  return normalize_r4vector(a * const_r4vector(c1) + b * const_r4vector(c2));
}

inline r4transform hyperbolic_translation(r4vector a, r4vector b) {
  r4transform rm = hyperbolic_reflection(hyperbolic_midpoint(a, b));
  r4transform ra = hyperbolic_reflection(a);

  return multiply_transformations(rm, ra);
}

inline r4vector weierstrass2poincare(r4vector a) {
  r4vector result = {a[0] / (a[2] + 1), a[1] / (a[2] + 1), 0, 0};
  return result;
}

inline r4vector poincare2weierstrass(r4vector a) {
  r4vector co = const_r4vector(1.0 / (1 - a[0]*a[0] - a[1]*a[1]));
  r4vector result = {2 * a[0], 2 * a[1], 1 + a[0] * a[0] + a[1] * a[1], 0 };
  return co * result;
}

inline r4vector weierstrass2klein(r4vector a) {
  r4vector result = {a[0] / a[2], a[1] / a[2], 1, 0};
  return result;
}

inline r4vector klein2weierstrass(r4vector a) {
  r4vector co = const_r4vector(1.0 / sqrt(1 - a[0]*a[0] - a[1]*a[1]));
  r4vector result = {a[0], a[1], 1, 0};
  return co * result;
}

#endif /* __HYPERBAN_MATRIX_H */
