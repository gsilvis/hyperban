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
#include <getopt.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <cairo.h>

#include "renderer.h"
#include "graph.h"
#include "matrix.h"
#include "level.h"
#include "build.h"
#include "consts.h"
#include "sokoban.h"

#define RENDERER_MIN_WIDTH 480
#define RENDERER_MIN_HEIGHT 480

#define MAX_ARC_RADIUS 10E4

static HyperbolicProjection projection = DEFAULT_PROJECTION;

/* returns points in clockwise order */
static SquarePoints *get_origin_square(void) {
  SquarePoints *res = malloc(sizeof(SquarePoints));
  *res = (SquarePoints) {
    {
      { MORE_MAGIC, -MORE_MAGIC, 0, 1},
      { MORE_MAGIC,  MORE_MAGIC, 0, 1},
      {-MORE_MAGIC,  MORE_MAGIC, 0, 1},
      {-MORE_MAGIC, -MORE_MAGIC, 0, 1}
    }
  };
  return res;
}

/* I just took this from wikipedia:Circumscribed_circle */
static void arc_to(cairo_t *cr, double x1, double y1, double x2,
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

static void draw_tile(RendererParams *params, SquarePoints *points, Tile *tile) {
  cairo_save(params->cr);
  cairo_translate(params->cr, params->origin[0], params->origin[1]);
  cairo_scale(params->cr, params->scale, params->scale);
  if (params->projection == PROJECTION_KLEIN) {
    for (size_t i = 0; i < 4; i++) {
      r4vector projected = points->points[i];
      cairo_line_to(params->cr, projected[0], projected[1]);
    }
    cairo_close_path(params->cr);
  } else if (params->projection == PROJECTION_POINCARE) {
    r4vector projected[4] = {
      klein2poincare(points->points[0]), klein2poincare(points->points[1]),
      klein2poincare(points->points[2]), klein2poincare(points->points[3])
    };
    r4vector midpoints[4] = {
      klein2poincare(hyperbolic_midpoint(points->points[0], points->points[1])),
      klein2poincare(hyperbolic_midpoint(points->points[1], points->points[2])),
      klein2poincare(hyperbolic_midpoint(points->points[2], points->points[3])),
      klein2poincare(hyperbolic_midpoint(points->points[3], points->points[0]))
    };
    cairo_move_to(params->cr, projected[0][0], projected[0][1]);
    for (size_t i = 0; i < 4; i++) {
      arc_to(params->cr, projected[i][0], projected[i][1], midpoints[i][0],
          midpoints[i][1], projected[(i+1)%4][0], projected[(i+1)%4][1]);
    }
    cairo_close_path(params->cr);
  }
  if (tile->tile_type == TILE_TYPE_SPACE) {
    if (tile->agent == AGENT_BOX)
      cairo_set_source_rgb(params->cr, 0, 0, 1);
    else
      cairo_set_source_rgb(params->cr, 1, 1, 1);
  } else if (tile->tile_type == TILE_TYPE_TARGET) {
    if (tile->agent == AGENT_BOX)
      cairo_set_source_rgb(params->cr, 0, 1, 0);
    else
      cairo_set_source_rgb(params->cr, 1, 1, 0);
  } else if (tile->tile_type == TILE_TYPE_WALL) {
    cairo_set_source_rgb(params->cr, .25, .25, .25);
  }

  cairo_fill_preserve(params->cr);
  cairo_set_source_rgb(params->cr, 0, 0, 0);
  cairo_stroke(params->cr);
  cairo_restore(params->cr);
}

static void add_queue(GraphQueue **queue, GraphQueue **queue_end,
    Graph *graph, SquarePoints* points, size_t dist) {
  GraphQueue *n = malloc(sizeof(GraphQueue));
  n->next = NULL;
  n->val = graph;
  n->points = points;
  n->dist = dist;
  if (*queue == NULL) {
    *queue = *queue_end = n;
  } else {
    *queue_end = ((*queue_end)->next = n);
  }
}

static void render_graph(RendererParams *params, Graph *graph) {
  GraphQueue *queue = malloc(sizeof(GraphQueue));
  GraphQueue *queue_end = queue;
  queue->val = graph;
  queue->next = NULL;
  queue->dist = 1;
  queue->points = get_origin_square();

  while (queue != NULL) {
    Graph *current = queue->val;
    SquarePoints *points = queue->points;
    GraphQueue *n = queue->next;
    size_t d = queue->dist;
    free(queue);
    queue = n;

    current->tile->dfs_use = 1;

    draw_tile(params, points, current->tile);

    if (d >= RENDERER_MAX_DIST) {
      free(points);
      continue;
    }

    if (current->adjacent && !current->adjacent->tile->dfs_use) {
      r4transform trans =
          hyperbolic_reflection(hyperbolic_midpoint(points->points[0],
              points->points[3]));
      SquarePoints *next_points = malloc(sizeof(SquarePoints));
      next_points->points[0] = apply_transformation(points->points[2], trans);
      next_points->points[1] = points->points[0];
      next_points->points[2] = points->points[3];
      next_points->points[3] = apply_transformation(points->points[1], trans);
      add_queue(&queue, &queue_end, ROTATE_B(current->adjacent), next_points,
          d+1);
    }

    if (current->rotate_r->adjacent &&
        !current->rotate_r->adjacent->tile->dfs_use) {
      r4transform trans =
          hyperbolic_reflection(hyperbolic_midpoint(points->points[0],
              points->points[1]));
      SquarePoints *next_points = malloc(sizeof(SquarePoints));
      next_points->points[0] = apply_transformation(points->points[2], trans);
      next_points->points[1] = apply_transformation(points->points[3], trans);
      next_points->points[2] = points->points[1];
      next_points->points[3] = points->points[0];
      add_queue(&queue, &queue_end, current->rotate_r->adjacent->rotate_r,
        next_points, d+1);
    }

    if (ROTATE_B(current)->adjacent &&
        !ROTATE_B(current)->adjacent->tile->dfs_use) {
      r4transform trans =
          hyperbolic_reflection(hyperbolic_midpoint(points->points[1],
              points->points[2]));
      SquarePoints *next_points = malloc(sizeof(SquarePoints));
      next_points->points[0] = points->points[1];
      next_points->points[1] = apply_transformation(points->points[3], trans);
      next_points->points[2] = apply_transformation(points->points[0], trans);
      next_points->points[3] = points->points[2];
      add_queue(&queue, &queue_end, ROTATE_B(current)->adjacent,
        next_points, d+1);
    }

    if (ROTATE_L(current)->adjacent &&
        !ROTATE_L(current)->adjacent->tile->dfs_use) {
      r4transform trans =
          hyperbolic_reflection(hyperbolic_midpoint(points->points[2],
              points->points[3]));
      SquarePoints *next_points = malloc(sizeof(SquarePoints));
      next_points->points[0] = points->points[3];
      next_points->points[1] = points->points[2];
      next_points->points[2] = apply_transformation(points->points[0], trans);
      next_points->points[3] = apply_transformation(points->points[1], trans);
      add_queue(&queue, &queue_end, ROTATE_L(ROTATE_L(current)->adjacent),
          next_points, d+1);
    }
    free(points);
  }
}

static void renderer_draw(GtkWidget *widget, Graph* graph) {
  cairo_t *cr;

  cr = gdk_cairo_create(widget->window);

  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_width(cr, 2);

  GtkRequisition requisition;

  gtk_widget_size_request(widget, &requisition);

  gint originx = requisition.width / 2;
  gint originy = requisition.height / 2;

  cairo_arc(cr, originx, originy, requisition.width/2 - RENDERER_BORDER,
      0, 2 * M_PI);

  cairo_set_source_rgb(cr, .5, .5, .5);
  cairo_fill_preserve(cr);

  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_stroke(cr);

  clear_dfs(graph);

  RendererParams params = {
    cr,
    {originx, originy},
    requisition.width/2 - RENDERER_BORDER,
    projection
  };

  cairo_set_line_width(cr, 1/params.scale);

  render_graph(&params, graph);

  cairo_destroy(cr);

  return;
}

static gboolean on_renderer_expose_event(GtkWidget *widget,
    GdkEventExpose *event, gpointer data) {
  Board* board = data;
  renderer_draw(widget, board->graph);
  return FALSE; // do not propogate event
}

static gboolean on_renderer_key_press_event(GtkWidget *widget,
    GdkEventKey *event, gpointer data) {
  Board* board = data;

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
  default:
    m = -1;
    break;
  }

  if (m != -1) {
    perform_move(board, m);
    gdk_window_invalidate_rect(widget->window, NULL, FALSE);
  }
  return FALSE;
}

static GtkWidget *get_renderer_widget(Board* board) {
  GtkWidget *result = gtk_event_box_new();

  gtk_widget_add_events(result, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

  g_signal_connect(result, "expose-event",
      G_CALLBACK(on_renderer_expose_event), board);
  g_signal_connect(result, "key-press-event",
      G_CALLBACK(on_renderer_key_press_event), board);

  gtk_widget_set_size_request(result, RENDERER_MIN_WIDTH, RENDERER_MIN_HEIGHT);

  gtk_widget_set_app_paintable(result, TRUE);

  gtk_widget_set_can_focus(result, TRUE);

  return result;
}

int main(int argc, char *argv[]) {
  if (!g_thread_supported()) {
    g_thread_init(NULL);
  }
  gdk_threads_init();
  gdk_threads_enter();

  gtk_init(&argc, &argv);

  char* level = NULL;

  while (1) {
    static struct option long_options[] = {
      {"level", required_argument, 0, 'l'},
      {"poincare", no_argument, 0, 'P'},
      {"klein", no_argument, 0, 'K'},
      {0, 0, 0, 0}
    };
    int option_index = 0;
    int c = getopt_long(argc, argv, "l:PK", long_options, &option_index);
    if (c == -1) break;
    switch(c) {
    case 'l':
      level = optarg;
      break;
    case 'P':
      projection = PROJECTION_POINCARE;
      break;
    case 'K':
      projection = PROJECTION_KLEIN;
      break;
    case '?':
      break;
    default:
      abort();
    }
  }

  if (level == NULL) {
    fprintf(stderr, "Required argument level not specified.\n");
    return 1;
  }

  FILE* levelfh = fopen(level, "r");
  if (levelfh == NULL) {
    perror("Could not open level!\n");
    return 1;
  }

  int num_tiles, unsolved;

  SavedTile *map = level_parse_file(levelfh, &num_tiles, &unsolved);

  fclose(levelfh);

  if (map == NULL) {
    fprintf(stderr, "Could not succesfully parse %s.\n", level);
    return 1;
  }

  Graph* graph = build_graph(map, num_tiles);

  if (graph == NULL) {
    fprintf(stderr, "Could not succesfully create graph from %s.\n", level);
    return 1;
  }

  Board board = {graph, unsolved};

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit),
      NULL);

  GtkWidget *renderer = get_renderer_widget(&board);
  gtk_container_add(GTK_CONTAINER(window), renderer);

  gtk_widget_show_all(window);

  gtk_widget_grab_focus(renderer);

  gtk_main();
  gdk_threads_leave();

  return 0;
}
