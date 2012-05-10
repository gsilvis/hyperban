#include <gtk/gtk.h>
#include <cairo.h>

#define RENDERER_MIN_WIDTH 480
#define RENDERER_MIN_HEIGHT 480

static gboolean on_renderer_expose_event(GtkWidget *widget,
  GdkEventExpose *event, gpointer data) {
  cairo_t *cr;

  cr = gdk_cairo_create(widget->window);

  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width(cr, 0.5);

  // etc.

  return FALSE; // do not propogate event
}

static GtkWidget *get_renderer_widget(void) {
  GtkWidget *result = gtk_event_box_new();

  gtk_widget_add_events(result, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

  g_signal_connect(result, "expose-event",
    G_CALLBACK(on_renderer_expose_event), NULL);

  gtk_widget_set_size_request(result, RENDERER_MIN_WIDTH, RENDERER_MIN_HEIGHT);

  gtk_widget_set_app_paintable(result, TRUE);

  return result;
}

int main(int argc, char *argv[]) {
  if (!g_thread_supported()) { g_thread_init(NULL); }
  gdk_threads_init();
  gdk_threads_enter();

  gtk_init(&argc, &argv);

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit),
    NULL);

  GtkWidget *renderer = get_renderer_widget();
  gtk_container_add(GTK_CONTAINER(window), renderer);

  gtk_widget_show_all(window);

  gtk_main();
  gdk_threads_leave();

  return 0;
}
