#ifndef __HYPERBAN_MATRIX_H
#define __HYPERBAN_MATRIX_H

#include "math.h"

typedef float matrix_el_t;

typedef matrix_el_t r4vector __attribute__ ((vector_size(sizeof(float)*4)));

typedef matrix_el_t r4transform __attribute__((vector_size(sizeof(float)*4*4)));

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
  r4transform result = {
    a[0] * b[0], a[0] * b[1], a[0] * b[2], a[0] * b[3],
    a[1] * b[0], a[1] * b[1], a[1] * b[2], a[1] * b[3],
    a[2] * b[0], a[2] * b[1], a[2] * b[2], a[2] * b[3],
    a[3] * b[0], a[3] * b[1], a[3] * b[2], a[3] * b[3]
  };
  return result;
}

inline r4vector apply_transformation(r4vector b, r4transform a) {
  r4vector result = {
    a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3],
    a[4] * b[0] + a[5] * b[1] + a[6] * b[3] + a[7] * b[3],
    a[8] * b[0] + a[9] * b[1] + a[10] * b[3] + a[11] * b[3],
    a[12] * b[0] + a[13] * b[1] + a[14] * b[3] + a[15] * b[3]
  };
  return result;
};

inline r4transform multiply_transformations(r4transform a, r4transform b) {
  r4transform result = {
    a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12],
    a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13],
    a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14],
    a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15],

    a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12],
    a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13],
    a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14],
    a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15],

    a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12],
    a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13],
    a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14],
    a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15],

    a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12],
    a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13],
    a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14],
    a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15]
  };
  return result;
}

inline r4vector normalize_r4vector(r4vector a) {
  r4vector result = { a[0]/a[3], a[1]/a[3], a[2]/a[3], 1 };
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

  result *= const_r4transform(2 / denom);

  return identity_transform() - result;
}

inline r4vector hyperbolic_midpoint(r4vector a, r4vector b) {
  matrix_el_t t1 = minkowski_inner_product(a, a);
  matrix_el_t t2 = minkowski_inner_product(b, b);
  matrix_el_t t3 = minkowski_inner_product(a, b);

  matrix_el_t c1 = sqrt(t2 + t3);
  matrix_el_t c2 = sqrt(t1 + t3);

  return a * const_r4vector(c1) + b * const_r4vector(c2);
}

inline r4transform hyperbolic_transformation(r4vector a, r4vector b) {
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
