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

#ifndef __HYPERBAN_WIDGETS_H
#define __HYPERBAN_WIDGETS_H

#include <gtk/gtk.h>

#include "./rendering.h"
#include "../graph/build.h"
#include "../graph/types.h"

/* Yes, I am aware this should really just extend EventBox */
struct renderer_widget_options_t {
  HyperbolicProjection projection;
  Board* board;
  gboolean drawing;
  gboolean animation;
  double scale;
  Move move; // simply to ease pasing a pointer to this struct to a thread
  GtkWidget *widget;
  GdkPixmap *pixmap;
  GtkLabel *moves_label;
  GtkLabel *boxes_label;
};

typedef struct renderer_widget_options_t RendererWidgetOptions;

void free_renderer_options(RendererWidgetOptions *o);

GtkWidget *get_renderer_widget(RendererWidgetOptions *opts);

#endif /* __HYPERBAN_WIDGETS_H */
