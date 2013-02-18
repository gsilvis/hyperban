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
#include <gtk/gtk.h>

#include "gui/widgets.h"
#include "renderer.h"
#include "graph/graph.h"
#include "gui/matrix.h"
#include "graph/level.h"
#include "graph/sokoban.h"
#include "graph/board.h"
#include "graph/generator.h"

static RendererWidgetOptions *parse_args(int argc, char *argv[]) {
  GError *error = NULL;
  gboolean klein = FALSE;
  gboolean poincare = FALSE;
  gboolean editing = FALSE;
  gchar** levels = NULL;
  gchar *level = NULL;
  double scale = 1;
  gchar* scale_str = NULL;
  HyperbolicProjection projection = DEFAULT_PROJECTION;
  gboolean animation = FALSE;
  gboolean noanimation = FALSE;
  gboolean random = FALSE;

  GOptionContext *context = g_option_context_new(NULL);
  g_option_context_set_summary(context, RENDERER_SUMMARY);
  GOptionEntry options[] = {
    {"poincare", 'P', 0, G_OPTION_ARG_NONE, &poincare,
        "Poincare Projection", NULL},
    {"klein", 'K', 0, G_OPTION_ARG_NONE, &klein,
        "Klein Projection (takes precedence)", NULL},
    {"animation", 'A', 0, G_OPTION_ARG_NONE, &animation,
        "Animate Moves", NULL},
    {"no-animation", 'N', 0, G_OPTION_ARG_NONE, &noanimation,
        "Don't Animate Moves", NULL},
    {"scale", 'S', 0, G_OPTION_ARG_STRING, &scale_str,
        "Render at a lower resolution (SCALE of 2 means scale"
        " rendered image x2 before displaying)", "SCALE"},
    {"random", 'R', 0, G_OPTION_ARG_NONE, &random,
        "Generate random level", NULL},
    {"editing", 'E', 0, G_OPTION_ARG_NONE, &editing,
        "Editing mode", NULL},
    {G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &levels,
        "Level File", "LEVEL"},
    { NULL, 0, 0, 0, NULL, NULL, NULL }
  };
  g_option_context_add_main_entries(context, options, NULL);
  g_option_context_add_group(context, gtk_get_option_group(TRUE));
  gboolean res = g_option_context_parse(context, &argc, &argv, &error);
  g_option_context_free(context);

  if (random) {
    if (!res) {
      fprintf(stderr, "Invalid command line options!\n");
      goto FAIL;
    }
  } else { /* !random */
    if (!res || levels == NULL || levels[0] == NULL || levels[1] != NULL) {
      fprintf(stderr, "Invalid command line options!\n");
      goto FAIL;
    }
  }

  if ((klein && poincare) || (animation && noanimation) || (random && level)) {
    fprintf(stderr, "Mutually exclusive command line options specified!\n");
    goto FAIL;
  }

  if (klein) {
    projection = PROJECTION_KLEIN;
  } else if (poincare) {
    projection = PROJECTION_POINCARE;
  }

  if (noanimation) {
    animation = FALSE;
  } else if (animation) {
    animation = TRUE;
  } else {
    animation = DEFAULT_ANIMATION;
  }

  if (scale_str != NULL) {
    if (!sscanf(scale_str, "%lf", &scale)) {
      fprintf(stderr, "Could not parse scale!\n");
    }
  }

  Board *board;

  if (random) {
    board = generate_board(NULL);
  } else {
    level = levels[0];
    FILE* levelfh = fopen(level, "r");
    if (levelfh == NULL) {
      perror("Could not open level");
      goto FAIL;
    }

    SavedTile *map = NULL;
    ConfigOption *cfg = NULL;
    level_parse_file(levelfh, &map, &cfg);

    fclose(levelfh);

    if ((map == NULL) || (cfg == NULL)) {
      fprintf(stderr, "Could not succesfully parse %s.\n", level);
      free(map);
      free(cfg);
      goto FAIL;
    }
    board = board_assemble_full(map, cfg);
    free(map);
    free(cfg);
    if (board == NULL) {
      fprintf(stderr, "Could not succesfully create board from %s.\n", level);
      goto FAIL;
    }
  }

  if (levels)
    g_strfreev(levels);
  if (scale_str)
    g_free(scale_str);

  RendererWidgetOptions *opts =
      malloc(sizeof(RendererWidgetOptions));
  opts->projection = projection;
  opts->board = board;
  g_atomic_int_set(&opts->drawing, FALSE);
  opts->animation = animation;
  opts->editing = editing;
  opts->scale = scale;

  return opts;
FAIL:
  if (levels)
    g_strfreev(levels);
  if (scale_str)
    g_free(scale_str);
  return NULL;
}

int main(int argc, char *argv[]) {
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

  free_renderer_options(opts);
  gdk_threads_leave();

  return 0;
}
