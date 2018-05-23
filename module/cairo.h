#ifndef _CAIRO_H
#define _CAIRO_H

typedef void * cairo_t;

void cairo_scale (cairo_t *cr, double sx, double sy);
void cairo_translate (cairo_t *cr, double tx, double ty);
void cairo_line_to (cairo_t *cr, double x, double y);
void cairo_arc (cairo_t *cr,
	   double xc, double yc,
	   double radius,
	   double angle1, double angle2);

void cairo_arc_negative (cairo_t *cr,
		    double xc, double yc,
		    double radius,
double angle1, double angle2);

void cairo_close_path (cairo_t *cr);

void cairo_move_to (cairo_t *cr, double x, double y);

void cairo_stroke (cairo_t *cr);
void cairo_fill (cairo_t *cr);

void cairo_paint (cairo_t *cr);

typedef enum _cairo_line_cap {
    CAIRO_LINE_CAP_BUTT,
    CAIRO_LINE_CAP_ROUND,
    CAIRO_LINE_CAP_SQUARE
} cairo_line_cap_t;

void cairo_set_line_cap (cairo_t *cr, cairo_line_cap_t line_cap);

void cairo_fill_preserve (cairo_t *cr);

void cairo_set_line_width (cairo_t *cr, double width);

void cairo_set_source_rgb (cairo_t *cr, double red, double green, double blue);

void cairo_curve_to (cairo_t *cr, double x1, double y1, double x2, double y2, double x3, double y3);

#endif

