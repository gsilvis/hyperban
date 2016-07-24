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

#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <cairo.h>
#include <sys/time.h>
#include <time.h>

#include "./widgets.h"
#include "../renderer.h"
#include "../graph/graph.h"
#include "./matrix.h"
#include "../graph/level.h"
#include "../graph/sokoban.h"
#include "../graph/board.h"
#include "./rendering.h"
#include "./cairo_helper.h"
#include "../graph/generator.h"
#include "../graph/serialize.h"

static double get_time(void) {
  struct timespec now;
  /* CLOCK_THREAD_CPUTIME_ID so I can compare values (inside a thread) */
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);
  return now.tv_sec + now.tv_nsec * 10E-9;
}

void free_renderer_options(RendererWidgetOptions *o) {
  if (o->board)
    free_board(o->board);
  if (o->pixmap)
    g_object_unref(o->pixmap);
  free(o);
}

static void draw_tile(RendererParams *params, SquarePoints *points,
    Tile *tile) {
  cairo_t *cr = params->data;
  if (params->projection == PROJECTION_KLEIN) {
    for (size_t i = 0; i < 4; i++) {
      r3vector projected = points->points[i];
      cairo_line_to(cr, projected[0], projected[1]);
    }
    cairo_close_path(cr);
  } else if (params->projection == PROJECTION_POINCARE) {
    r3vector projected[4] = {
      klein2poincare(points->points[0]),
      klein2poincare(points->points[1]),
      klein2poincare(points->points[2]),
      klein2poincare(points->points[3])
    };
    r3vector midpoints[4] = {
      klein2poincare(hyperbolic_midpoint(points->points[0], points->points[1])),
      klein2poincare(hyperbolic_midpoint(points->points[1], points->points[2])),
      klein2poincare(hyperbolic_midpoint(points->points[2], points->points[3])),
      klein2poincare(hyperbolic_midpoint(points->points[3], points->points[0]))
    };
    cairo_move_to(cr, projected[0][0], projected[0][1]);
    for (size_t i = 0; i < 4; i++) {
      circle_to(cr, projected[i][0], projected[i][1], midpoints[i][0],
          midpoints[i][1], projected[(i+1)%4][0], projected[(i+1)%4][1]);
    }
    cairo_close_path(cr);
  }
  if (tile->tile_type == TILE_TYPE_SPACE) {
    if (tile->agent == AGENT_BOX)
      cairo_set_source_rgb(cr, 0, 0, 1);
    else
      cairo_set_source_rgb(cr, 1, 1, 1);
  } else if (tile->tile_type == TILE_TYPE_TARGET) {
    if (tile->agent == AGENT_BOX)
      cairo_set_source_rgb(cr, 0, 1, 0);
    else
      cairo_set_source_rgb(cr, 1, 1, 0);
  } else if (tile->tile_type == TILE_TYPE_WALL) {
    cairo_set_source_rgb(cr, .25, .25, .25);
  }

  cairo_fill_preserve(cr);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_stroke(cr);
}

static void renderer_draw(cairo_t *cr, double width, double height,
    Graph* graph, HyperbolicProjection projection, Move m, double frame) {
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);

  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_width(cr, 2);

  double radius = fmin(width, height)/2 - RENDERER_BORDER;

  double originx = width / 2;
  double originy = height / 2;

  cairo_arc(cr, originx, originy, radius,
      0, 2 * M_PI);

  cairo_set_source_rgb(cr, .5, .5, .5);
  cairo_fill_preserve(cr);

  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_stroke(cr);

  clear_search(graph);

  SquarePoints *origin = new_squarepoints();
  *origin = origin_square;

  if (frame != 0) {
    SquarePoints* next = move_square(origin, (m + 2) % 4);
    r3vector from = {0, 0, 1};
    r3vector to = hyperbolic_midpoint(
        hyperbolic_midpoint(next->points[0], next->points[1]),
        hyperbolic_midpoint(next->points[2], next->points[3]));
    to = to * const_r3vector(frame);
    to[2] = 1;
    r3transform transformation;
    hyperbolic_translation(from, to, &transformation);
    SquarePoints *temp = transform_square(origin, &transformation);
    free(origin);
    free(next);
    origin = temp;
  }

  RendererParams params = {
    draw_tile,
    {originx, originy},
    radius,
    projection,
    origin,
    cr
  };

  cairo_set_line_width(cr, 1/params.scale);

  cairo_translate(cr, params.origin[0], params.origin[1]);
  cairo_scale(cr, params.scale, params.scale);

  render_graph(&params, graph);

  free(origin);

  return;
}

static void set_labels(RendererWidgetOptions *opts) {
  char *t;
  t = g_strdup_printf(BOXES_TEXT, opts->board->unsolved);
  gtk_label_set_text(opts->boxes_label, t);
  g_free(t);

  t = g_strdup_printf(MOVES_TEXT, opts->board->number_moves);
  gtk_label_set_text(opts->moves_label, t);
  g_free(t);
}

static gpointer draw_thread(gpointer ptr) {
  RendererWidgetOptions *opts = ptr;
  if (opts->pixmap == NULL) return NULL;

  int width, height, width2, height2;

  cairo_matrix_t mat;

  cairo_matrix_init_scale(&mat, 1.0/opts->scale, 1.0/opts->scale);

 rerender:
  gdk_threads_enter();
  gdk_pixmap_get_size(opts->pixmap, &width, &height);
  gdk_threads_leave();

  cairo_surface_t *cst = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
      width / opts->scale, height / opts->scale);
  Graph* oldpos = opts->board->graph;
  int res = 0;
  if (opts->move >= 0) {
    res = (perform_move(opts->board, opts->move) != RESULT_NO_MOVE_POSSIBLE);
  } else if (opts->move == -2) {
    res = (unperform_move(opts->board));
    opts->move = -1;
    switch(res) {
    case 'r': case 'R':
      opts->move++;
    case 'u': case 'U':
      opts->move++;
    case 'l': case 'L':
      opts->move++;
    case 'd': case 'D':
      opts->move++;
    }
  }
  if (res && opts->animation) {
    double start = get_time();

    while (TRUE) {
      double frame_start = get_time();
      cairo_t *cr = cairo_create(cst);

      double frame = (fmin(RENDERER_ANIMATION_TIME, frame_start-start) /
          RENDERER_ANIMATION_TIME);

      renderer_draw(cr, width / opts->scale, height / opts->scale, oldpos,
          opts->projection, opts->move, frame);

      cairo_destroy(cr);

      gdk_threads_enter();

      cairo_t *cr_win = gdk_cairo_create(opts->pixmap);
      cairo_set_source_rgb(cr_win, 1, 1, 1);
      cairo_paint(cr_win);
      cairo_set_source_surface(cr_win, cst, 0, 0);
      cairo_pattern_set_matrix(cairo_get_source(cr_win), &mat);
      cairo_pattern_set_filter(cairo_get_source(cr_win), RENDERER_INTERP_MODE);
      cairo_paint(cr_win);
      cairo_destroy(cr_win);

      gdk_window_invalidate_rect(opts->widget->window, NULL, FALSE);
      gdk_threads_leave();

      double frame_end = get_time();
      if (frame_end - start > RENDERER_ANIMATION_TIME) {
        break;
      }
      if (frame_end - frame_start < 1.0/RENDERER_MAX_FRAME_RATE) {
        double sleeptime = 1.0/RENDERER_MAX_FRAME_RATE -
            (frame_end - frame_start);
        struct timespec sleep;
        sleep.tv_sec = sleeptime;
        sleep.tv_nsec = fmin(999999999, sleeptime/10E-9);
        nanosleep(&sleep, NULL);
      }
    }
  }
  cairo_t *cr = cairo_create(cst);

  renderer_draw(cr, width / opts->scale, height / opts->scale,
      opts->board->graph, opts->projection, 0, 0);

  cairo_destroy(cr);

  gdk_threads_enter();

  cairo_t *cr_win = gdk_cairo_create(opts->pixmap);
  cairo_set_source_rgb(cr_win, 1, 1, 1);
  cairo_paint(cr_win);
  cairo_set_source_surface(cr_win, cst, 0, 0);
  cairo_pattern_set_matrix(cairo_get_source(cr_win), &mat);
  cairo_pattern_set_filter(cairo_get_source(cr_win), RENDERER_INTERP_MODE);
  cairo_paint(cr_win);
  cairo_destroy(cr_win);

  gdk_window_invalidate_rect(opts->widget->window, NULL, FALSE);

  set_labels(opts);
  gdk_pixmap_get_size(opts->pixmap, &width2, &height2);
  gdk_threads_leave();

  cairo_surface_destroy(cst);

  if (width != width2 || height != height2) {
    goto rerender;
  }

  g_atomic_int_set(&opts->drawing, FALSE);

  return NULL;
}

static void animate_move(RendererWidgetOptions *opts, Move m) {
  if (g_atomic_int_get(&opts->drawing)) return;
  g_atomic_int_set(&opts->drawing, TRUE);
  opts->move = m;
  g_thread_unref(g_thread_new("draw thread", draw_thread, opts));
}

static void toggle_help(RendererWidgetOptions *opts) {
  if (gtk_widget_get_visible(opts->help)) {
    gtk_widget_hide(opts->help);
  } else {
    gtk_widget_show(opts->help);
  }
  // Force rendering widget to update perhaps changed allocation size.
  gtk_main_iteration_do(FALSE);
}

static gboolean on_renderer_expose_event(GtkWidget *widget,
    GdkEventExpose *event, gpointer data) {
  RendererWidgetOptions *opts = data;
  if (opts->pixmap) {
    gdk_draw_drawable(widget->window,
        widget->style->fg_gc[GTK_WIDGET_STATE(widget)], opts->pixmap,
        event->area.x, event->area.y,
        event->area.x, event->area.y,
        event->area.width, event->area.height);
  }
  return FALSE;
}

static gboolean on_renderer_key_press_event(GtkWidget *widget,
    GdkEventKey *event, gpointer data) {
  RendererWidgetOptions *opts = data;
  if (g_atomic_int_get(&opts->drawing)) return TRUE;

  Move m = -1;

  switch(event->keyval) {
  case KEY_UP:
    m = MOVE_UP;
    break;
  case KEY_DOWN:
    m = MOVE_DOWN;
    break;
  case KEY_LEFT:
    m = MOVE_LEFT;
    break;
  case KEY_RIGHT:
    m = MOVE_RIGHT;
    break;
  case KEY_UNDO:
    if (!opts->editing)
      m = -2;
    break;
  case KEY_MAKE_FLOOR:
    if (opts->editing) {
      opts->board->graph->adjacent->tile->tile_type = TILE_TYPE_SPACE;
      build_wall_in(opts->board->graph->adjacent);
    }
    break;
  case KEY_MAKE_WALL:
    if (opts->editing) {
      opts->board->graph->adjacent->tile->tile_type = TILE_TYPE_WALL;
    }
    break;
  case KEY_ROT_LEFT:
    if (opts->editing)
      opts->board->graph = ROTATE_L(opts->board->graph);
    break;
  case KEY_ROT_RIGHT:
    if (opts->editing)
      opts->board->graph = opts->board->graph->rotate_r;
    break;
  case KEY_MAKE_BOX:
    if (opts->editing) {
      if (opts->board->graph->adjacent->tile->agent != AGENT_BOX) {
        opts->board->graph->adjacent->tile->tile_type = TILE_TYPE_SPACE;
        build_wall_in(opts->board->graph->adjacent);
        opts->board->graph->adjacent->tile->agent = AGENT_BOX;
        opts->board->unsolved++;
      }
    }
    break;
  case KEY_DELETE_AGENT:
    if (opts->editing) {
      if (opts->board->graph->adjacent->tile->agent == AGENT_BOX) {
        opts->board->unsolved--;
      }
      opts->board->graph->adjacent->tile->agent = AGENT_NONE;
    }
    break;
  case KEY_MAKE_TARGET:
    if (opts->editing) {
      opts->board->graph->adjacent->tile->tile_type = TILE_TYPE_TARGET;
      build_wall_in(opts->board->graph->adjacent);
    }
    break;
  case KEY_SAVE:
    if (opts->editing) {
      FILE *f = fopen(opts->board->filename, "w");
      serialize_board(opts->board, f);
      fclose(f);
    }
    break;
  case KEY_HELP:
    toggle_help(opts);
    return FALSE;
  default:
    return FALSE;
  }

  animate_move(opts, m);
  return FALSE;
}

static gboolean on_renderer_realize(GtkWidget *widget, gpointer data) {
  RendererWidgetOptions *opts = data;

  GdkRectangle allocation;

  gtk_widget_get_allocation(widget, &allocation);

  int width = allocation.width, height = allocation.height;
  int pwidth = -1, pheight = -1;
  if (opts->pixmap != NULL) {
    gdk_pixmap_get_size(opts->pixmap, &pwidth, &pheight);
  }
  if (pwidth != width || pheight != height) {
    GdkPixmap *tmp = gdk_pixmap_new(widget->window, width,
        height, -1);
    if (opts->pixmap != NULL) {
      int minw = width;
      int minh = height;
      if(pwidth < minw && pwidth != -1) minw = pwidth;
      if(pheight < minh && pheight != -1) minh = pheight;
      gdk_draw_drawable(tmp, widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
          opts->pixmap, 0, 0, 0, 0, minw, minh);
      g_object_unref(opts->pixmap);
    }
    opts->pixmap = tmp;
    animate_move(opts, -1);
  }
  return FALSE;
}

static gboolean on_renderer_size_allocate(GtkWidget *widget,
    GdkRectangle* allocation, gpointer data) {
  if (gtk_widget_get_realized(widget))
    on_renderer_realize(widget, data);
  return FALSE;
}

GtkWidget *get_renderer_widget(RendererWidgetOptions *opts) {
  opts->pixmap = NULL;

  GtkWidget *result = gtk_event_box_new();

  gtk_widget_add_events(result, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

  g_signal_connect(result, "expose-event",
      G_CALLBACK(on_renderer_expose_event), opts);
  g_signal_connect(result, "key-press-event",
      G_CALLBACK(on_renderer_key_press_event), opts);
  g_signal_connect(result, "realize",
      G_CALLBACK(on_renderer_realize), opts);
  g_signal_connect(result, "size-allocate",
      G_CALLBACK(on_renderer_size_allocate), opts);

  gtk_widget_set_size_request(result, RENDERER_MIN_WIDTH, RENDERER_MIN_HEIGHT);

  gtk_widget_set_app_paintable(result, TRUE);
  gtk_widget_set_double_buffered(result, FALSE);

  gtk_widget_set_can_focus(result, TRUE);

  opts->widget = result;

  return result;
}
