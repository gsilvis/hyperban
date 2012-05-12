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
#include <cairo.h>

#include "renderer.h"
#include "graph.h"
#include "matrix.h"
#include "level.h"
#include "build.h"
#include "consts.h"

#define RENDERER_MIN_WIDTH 480
#define RENDERER_MIN_HEIGHT 480

/* returns points in clockwise order */
static SquarePoints get_origin_square(void) {
  SquarePoints res = {
    {
      { MORE_MAGIC,  MORE_MAGIC, 0, 1},
      { MORE_MAGIC, -MORE_MAGIC, 0, 1},
      {-MORE_MAGIC, -MORE_MAGIC, 0, 1},
      {-MORE_MAGIC,  MORE_MAGIC, 0, 1}
    }
  };
  return res;
}

static void draw_points(cairo_t *cr, matrix_el_t originx, matrix_el_t originy, matrix_el_t scale, SquarePoints points) {
  for (size_t i = 0; i < 4; i++) {
    r4vector projected = points.points[i];
    cairo_move_to(cr, originx+(scale*projected[0]), originy-(scale*projected[1]));
    cairo_line_to(cr, originx+(scale*projected[0]), originy-(scale*projected[1]));
    cairo_stroke(cr);
  }
}

static void render_recursive(cairo_t *cr, matrix_el_t originx, matrix_el_t originy, matrix_el_t scale, SquarePoints points, Graph *current) {
  current->tile->dfs_use = 1;

  draw_points(cr, originx, originy, scale, points);

  if (current->adjacent && !current->adjacent->tile->dfs_use) {
    r4transform trans = hyperbolic_reflection(hyperbolic_midpoint(points.points[0], points.points[3]));
    SquarePoints nextPoints = {
      {
        apply_transformation(points.points[2], trans),
        points.points[0],
        points.points[3],
        apply_transformation(points.points[1], trans)
      }
    };
    render_recursive(cr, originx, originy, scale, nextPoints, current->adjacent->rotate_r->rotate_r);
  }

  if (current->rotate_r->adjacent && !current->rotate_r->adjacent->tile->dfs_use) {
    r4transform trans = hyperbolic_reflection(hyperbolic_midpoint(points.points[0], points.points[1]));
    SquarePoints nextPoints = {
      {
        apply_transformation(points.points[2], trans),
        apply_transformation(points.points[3], trans),
        points.points[1],
        points.points[0]
      }
    };
    render_recursive(cr, originx, originy, scale, nextPoints, current->rotate_r->adjacent->rotate_r);
  }

  if (current->rotate_r->rotate_r->adjacent && !current->rotate_r->rotate_r->adjacent->tile->dfs_use) {
    r4transform trans = hyperbolic_reflection(hyperbolic_midpoint(points.points[1], points.points[2]));
    SquarePoints nextPoints = {
      {
        points.points[1],
        apply_transformation(points.points[3], trans),
        apply_transformation(points.points[0], trans),
        points.points[2]
      }
    };
    render_recursive(cr, originx, originy, scale, nextPoints, current->rotate_r->rotate_r->adjacent);
  }

  if (current->rotate_r->rotate_r->rotate_r->adjacent && !current->rotate_r->rotate_r->rotate_r->adjacent->tile->dfs_use) {
    r4transform trans = hyperbolic_reflection(hyperbolic_midpoint(points.points[2], points.points[3]));
    SquarePoints nextPoints = {
      {
        points.points[3],
        points.points[2],
        apply_transformation(points.points[0], trans),
        apply_transformation(points.points[1], trans)
      }
    };
    render_recursive(cr, originx, originy, scale, nextPoints, current->rotate_r->rotate_r->rotate_r->adjacent->rotate_r->rotate_r->rotate_r);
  }
}

static void clear_dfs(Graph* graph) {
  graph->tile->dfs_use = 0;
  if (graph->adjacent && graph->adjacent->tile->dfs_use) {
    clear_dfs(graph->adjacent);
  }
  if (graph->rotate_r->adjacent && graph->rotate_r->adjacent->tile->dfs_use) {
    clear_dfs(graph->rotate_r->adjacent);
  }
  if (graph->rotate_r->rotate_r->adjacent && graph->rotate_r->rotate_r->adjacent->tile->dfs_use) {
    clear_dfs(graph->rotate_r->rotate_r->adjacent);
  }
  if (graph->rotate_r->rotate_r->rotate_r->adjacent && graph->rotate_r->rotate_r->rotate_r->adjacent->tile->dfs_use) {
    clear_dfs(graph->rotate_r->rotate_r->rotate_r->adjacent);
  }
}

static gboolean on_renderer_expose_event(GtkWidget *widget,
    GdkEventExpose *event, gpointer data) {
  Graph* graph = *(Graph**)data;

  cairo_t *cr;

  cr = gdk_cairo_create(widget->window);

  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_width(cr, 0.5);

  GtkRequisition requisition;

  gtk_widget_size_request(widget, &requisition);

  gint originx = requisition.width / 2;
  gint originy = requisition.height / 2;

  cairo_arc(cr, originx, originy, requisition.width/2, 0, 2 * M_PI);

  cairo_stroke(cr);

  cairo_set_line_width(cr, 5);

  clear_dfs(graph);

  render_recursive(cr, originx, originy, requisition.width/2, get_origin_square(), graph);

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

  char* level = NULL;

  while (1) {
    static struct option long_options[] = {
      {"level", required_argument, 0, 'l'},
      {0, 0, 0, 0}
    };
    int option_index = 0;
    int c = getopt_long(argc, argv, "l:", long_options, &option_index);
    if (c == -1) break;
    switch(c) {
    case 'l':
      level = optarg;
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
