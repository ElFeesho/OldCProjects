#include <gtk/gtk.h>

static gboolean canvas_expose_event(GtkWidget *canvas, GdkEventExpose *ev, gpointer user_data);

GtkWidget *canvas_create(int canvas_num)
{
	GtkWidget *canvas = gtk_drawing_area_new();
	
	g_signal_connect(G_OBJECT(canvas),"expose-event",G_CALLBACK(canvas_expose_event),NULL);

	return canvas;
}

static gboolean canvas_expose_event(GtkWidget *canvas, GdkEventExpose *ev, gpointer user_data)
{
	cairo_t *cr = gdk_cairo_create(canvas->window);
	cairo_set_source_rgb(cr,1,1,1);
	cairo_move_to(cr,0,0);
	cairo_line_to(cr,canvas->allocation.width,0);
	cairo_line_to(cr,canvas->allocation.width,canvas->allocation.height);
	cairo_line_to(cr,0,canvas->allocation.height);
	cairo_line_to(cr,0,0);
	cairo_fill(cr);
	cairo_destroy(cr);
	return 1;
}
