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

#include "cairo_helper.h"

#include <math.h>

/* I just took this from wikipedia:Circumscribed_circle */
void circle_to(cairo_t *cr, double x1, double y1, double x2,
    double y2, double x3, double y3) {
  // Calculate the circumcenter of the triangle
  double _x2p = x2-x1;
  double _x3p = x3-x1;
  double _y2p = y2-y1;
  double _y3p = y3-y1;

  double d = 2 * (_x2p*_y3p - _y2p*_x3p);

  double _2s = (_x2p*_x2p + _y2p*_y2p);
  double _3s = (_x3p*_x3p + _y3p*_y3p);

  double cx = (_y3p*_2s - _y2p*_3s) / d + x1;
  double cy = (_x2p*_3s - _x3p*_2s) / d + y1;

  // Calculate the distance of the points from the circumcenter (i.e. radius)
  double t1 = x1 - cx;
  double t2 = y1 - cy;

  double r = sqrt(t1 * t1 + t2 * t2);

  if (r > MAX_ARC_RADIUS) {
    cairo_line_to(cr, x2, y2);
    cairo_line_to(cr, x3, y3);
    return;
  }

  // Calculate the angles required for the arc
  double a1 = atan2((y1 - cy), (x1 - cx));
  double a2 = atan2((y3 - cy), (x3 - cx));

  if (a1 < 0) a1 += 2*M_PI;
  if (a2 < 0) a2 += 2*M_PI;

  double diff = a2 - a1;

  if ((diff > 0 && diff < M_PI) || diff < -M_PI)
    cairo_arc(cr, cx, cy, r, a1, a2);
  else
    cairo_arc_negative(cr, cx, cy, r, a1, a2);
}

// This is from https://www.particleincell.com/2012/bezier-splines/
// specifically https://www.particleincell.com/wp-content/uploads/2012/06/bezier-spline.js
// expanded to take 5 input points instead of 4
static void computeControlPoints(double* inputs, double* p1, double* p2, int ninput) {
    int n = ninput - 1;

    double a[n], b[n], c[n], r[n];

    a[0] = 0;
    b[0] = 2;
    c[0] = 1;
    r[0] = inputs[0] + 2 * inputs[1];

    for (int i = 0; i < n - 1; i++) {
      a[i] = 1;
      b[i] = 4;
      c[i] = 1;
      r[i] = 4 * inputs[i] + 2 * inputs[i+1];
    }

    a[n-1] = 2;
    b[n-1] = 7;
    c[n-1] = 0;
    r[n-1] = 8 * inputs[n-1] + inputs[n];

    for (int i = 1; i < n; i++) {
        double m = a[i]/b[i-1];
        b[i] -= m * c[i - 1];
        r[i] -= m * r[i - 1];
    }

    p1[n-1] = r[n-1]/b[n-1];
    for (int i = n -2; i >= 0; i--) {
      p1[i] = (r[i] - c[i] * p1[i+1]) / b[i];
    }
    for (int i = 0; i < n - 1; i++) {
      p2[i] = 2 * inputs[i+1] - p1[i+1];
    }
    p2[n - 1] = 0.5 * (inputs[n] + p1[n-1]);

}

void spline_to(cairo_t *cr, double x[4], double y[4]) {
    int n = 4;

    double xValuesOut1[n-1];
    double xValuesOut2[n-1];
    double yValuesOut1[n-1];
    double yValuesOut2[n-1];

    computeControlPoints(x, xValuesOut1, xValuesOut2, n);
    computeControlPoints(y, yValuesOut1, yValuesOut2, n);

    for (int i = 0; i < n; i++) {
        cairo_curve_to(cr,
		xValuesOut1[i], yValuesOut1[i],
		xValuesOut2[i], yValuesOut2[i],
		x[i+1], y[i+1]);
    }
}
