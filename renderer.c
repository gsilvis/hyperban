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
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

#include "renderer.h"
#include "graph.h"
#include "matrix.h"
#include "level.h"
#include "build.h"
#include "sokoban.h"
#include "board.h"
#include "rendering.h"
#include "cairo_helper.h"

inline double get_time() {
  struct timespec now;
  /* CLOCK_THREAD_CPUTIME_ID so I can compare values (inside a thread) */
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);
  return now.tv_sec + now.tv_nsec * 10E-9;
}

/* Yes, I am aware this should really just extend EventBox */
struct renderer_widget_options_t {
  HyperbolicProjection projection;
  Board* board;
  gboolean drawing;
  gboolean animation;
  pthread_t thread;
  Move move; // simply to ease pasing a pointer to this struct to a thread
  GtkWidget *widget;
  GdkPixmap *pixmap;
  GtkLabel *moves_label;
  GtkLabel *boxes_label;
};

typedef struct renderer_widget_options_t RendererWidgetOptions;

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
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_width(cr, 2);

  double radius = fmin(width, height)/2 - RENDERER_BORDER;

  double originx = width / 2;
  double originy = height / 2;

  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_rectangle(cr, 0, 0, width, height);
  cairo_fill(cr);

  cairo_arc(cr, originx, originy, radius,
      0, 2 * M_PI);

  cairo_set_source_rgb(cr, .5, .5, .5);
  cairo_fill_preserve(cr);

  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_stroke(cr);

  clear_dfs(graph);

  SquarePoints *origin = get_origin_square();

  if (frame != 0) {
    SquarePoints* next = move_square(origin, (m + 2) % 4);
    r3vector from = {0, 0, 1};
    r3vector to = hyperbolic_midpoint(
        hyperbolic_midpoint(next->points[0], next->points[1]),
        hyperbolic_midpoint(next->points[2], next->points[3]));
    to = to * const_r3vector(frame);
    to[2] = 1;
    r3transform transformation = hyperbolic_translation(from, to);
    SquarePoints *temp = transform_square(origin, transformation);
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

void set_labels(RendererWidgetOptions *opts) {
  size_t s;
  char *t;
  s = snprintf(NULL, 0, BOXES_TEXT, opts->board->unsolved);
  t = malloc(sizeof(char)*s);
  sprintf(t, BOXES_TEXT, opts->board->unsolved);
  gtk_label_set_text(opts->boxes_label, t);
  free(t);

  s = snprintf(NULL, 0, MOVES_TEXT, opts->board->number_moves);
  t = malloc(sizeof(char)*s);
  sprintf(t, MOVES_TEXT, opts->board->number_moves);
  gtk_label_set_text(opts->moves_label, t);
  free(t);

}

void *draw_thread(void *ptr) {
  RendererWidgetOptions *opts = ptr;
  if (opts->pixmap == NULL) return NULL;

  int width, height;

  gdk_threads_enter();
  gdk_pixmap_get_size(opts->pixmap, &width, &height);
  gdk_threads_leave();

  cairo_surface_t *cst = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
      width, height);
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
    case 'b': case 'B':
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

      renderer_draw(cr, width, height, oldpos,
          opts->projection, opts->move, frame);

      cairo_destroy(cr);

      gdk_threads_enter();

      cairo_t *cr_win = gdk_cairo_create(opts->pixmap);
      cairo_set_source_surface(cr_win, cst, 0, 0);
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

  renderer_draw(cr, width, height, opts->board->graph, opts->projection, 0, 0);

  cairo_destroy(cr);

  gdk_threads_enter();

  cairo_t *cr_win = gdk_cairo_create(opts->pixmap);
  cairo_set_source_surface(cr_win, cst, 0, 0);
  cairo_paint(cr_win);
  cairo_destroy(cr_win);

  gdk_window_invalidate_rect(opts->widget->window, NULL, FALSE);

  set_labels(opts);

  gdk_threads_leave();

  cairo_surface_destroy(cst);

  g_atomic_int_set(&opts->drawing, FALSE);

  return NULL;
}

void animate_move(RendererWidgetOptions *opts, Move m) {
  g_atomic_int_set(&opts->drawing, TRUE);
  opts->move = m;
  pthread_create(&opts->thread, NULL, draw_thread, opts);
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

  Move m;

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
    m = -2;
    break;
  default:
    m = -1;
    break;
  }

  if (m != -1) {
    animate_move(opts, m);
  }
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

static GtkWidget *get_renderer_widget(RendererWidgetOptions *opts) {
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

RendererWidgetOptions *parse_args(int argc, char *argv[]) {
  GError *error = NULL;
  gboolean klein = FALSE;
  gboolean poincare = FALSE;
  gchar* level = NULL;
  HyperbolicProjection projection = DEFAULT_PROJECTION;
  gboolean animation = FALSE;
  gboolean noanimation = FALSE;

  GOptionContext *context = g_option_context_new(NULL);
  g_option_context_set_summary(context, RENDERER_SUMMARY);
  GOptionEntry options[] = {
    {"poincare", 'P', 0, G_OPTION_ARG_NONE, &poincare,
        "Poincare Projection", NULL},
    {"klein", 'K', 0, G_OPTION_ARG_NONE, &klein,
        "Klein Projection (takes precedence)", NULL},
    {"level", 'l', 0, G_OPTION_ARG_FILENAME, &level,
        "Level File", "LEVEL"},
    {"animation", 'A', 0, G_OPTION_ARG_NONE, &animation,
        "Animate Moves (takes precedence)", NULL},
    {"no-animation", 'N', 0, G_OPTION_ARG_NONE, &noanimation,
        "Don't Animate Moves", NULL},
    { NULL, 0, 0, 0, NULL, NULL, NULL }
  };
  g_option_context_add_main_entries(context, options, NULL);
  g_option_context_add_group(context, gtk_get_option_group(TRUE));
  g_option_context_parse(context, &argc, &argv, &error);
  g_option_context_free(context);

  if (klein || !poincare) {
    projection = PROJECTION_KLEIN;
  } else {
    projection = PROJECTION_POINCARE;
  }

  animation = (animation || (DEFAULT_ANIMATION && !noanimation));

  if (level == NULL) {
    fprintf(stderr, "Required argument level not specified.\n");
    return NULL;
  }

  FILE* levelfh = fopen(level, "r");
  if (levelfh == NULL) {
    perror("Could not open level!\n");
    return NULL;
  }


  SavedTile *map = NULL;
  ConfigOption *cfg = NULL;
  level_parse_file(levelfh, &map, &cfg);

  fclose(levelfh);

  if ((map == NULL) || (cfg == NULL)) {
    fprintf(stderr, "Could not succesfully parse %s.\n", level);
    return NULL;
  }

  Board* board = board_assemble_full(map, cfg);

  if (board == NULL) {
    fprintf(stderr, "Could not succesfully create board from %s.\n", level);
    return NULL;
  }

  RendererWidgetOptions *opts =
      malloc(sizeof(RendererWidgetOptions));
  opts->projection = projection;
  opts->board = board;
  g_atomic_int_set(&opts->drawing, FALSE);
  opts->animation = animation;

  return opts;
}

int main(int argc, char *argv[]) {
  if (!g_thread_supported()) {
    g_thread_init(NULL);
  }
  gdk_threads_init();
  gdk_threads_enter();

  RendererWidgetOptions *opts = parse_args(argc, argv);
  if (opts == NULL) return 1;

  GtkWidget *moves = gtk_label_new("Moves : ");
  GtkWidget *left = gtk_label_new("Boxes Left : ");

  opts->moves_label = GTK_LABEL(moves);
  opts->boxes_label = GTK_LABEL(left);

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), RENDERER_TITLE);
  gtk_container_set_border_width(GTK_CONTAINER(window), RENDERER_BORDER);
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit),
      NULL);

  GtkWidget *vbox = gtk_vbox_new(FALSE, RENDERER_BORDER);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  GtkWidget *renderer = get_renderer_widget(opts);
  gtk_box_pack_start(GTK_BOX(vbox), renderer, TRUE, TRUE, 0);

  GtkWidget *hbox = gtk_hbox_new(TRUE, 0);
  gtk_box_pack_end(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(hbox), moves, TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(hbox), left, TRUE, TRUE, 0);

  gtk_widget_show_all(window);

  gtk_widget_grab_focus(renderer);

  gtk_main();
  gdk_threads_leave();

  return 0;
}
