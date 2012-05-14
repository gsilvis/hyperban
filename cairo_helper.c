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

