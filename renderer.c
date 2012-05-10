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

#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>

#include "graph.h"
#include "matrix.h"

#define RENDERER_MIN_WIDTH 480
#define RENDERER_MIN_HEIGHT 480

static gboolean on_renderer_expose_event(GtkWidget *widget,
    GdkEventExpose *event, gpointer data) {
  Graph* graph = *(Graph**)data;

  cairo_t *cr;

  cr = gdk_cairo_create(widget->window);

  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width(cr, 0.5);

  GtkRequisition requisition;

  gtk_widget_size_request(widget, &requisition);

  gint originx = requisition.width / 2;
  gint originy = requisition.height / 2;

  cairo_arc(cr, originx, originy, requisition.width/2, 0, 2 * M_PI);

  cairo_stroke(cr);
  cairo_destroy(cr);

  return FALSE; // do not propogate event
}

static GtkWidget *get_renderer_widget(Graph** graph) {
  GtkWidget *result = gtk_event_box_new();

  gtk_widget_add_events(result, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

  g_signal_connect(result, "expose-event",
      G_CALLBACK(on_renderer_expose_event), graph);

  gtk_widget_set_size_request(result, RENDERER_MIN_WIDTH, RENDERER_MIN_HEIGHT);

  gtk_widget_set_app_paintable(result, TRUE);

  return result;
}

int main(int argc, char *argv[]) {
  if (!g_thread_supported()) {
    g_thread_init(NULL);
  }
  gdk_threads_init();
  gdk_threads_enter();

  gtk_init(&argc, &argv);

  Graph* graph = NULL;

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit),
      NULL);

  GtkWidget *renderer = get_renderer_widget(&graph);
  gtk_container_add(GTK_CONTAINER(window), renderer);

  gtk_widget_show_all(window);

  gtk_main();
  gdk_threads_leave();

  return 0;
}
