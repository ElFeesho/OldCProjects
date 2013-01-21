#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "globals.h"
#include "draw.h"

static int x, y;
static GtkWidget *zoom_win;
/* Callback Declarations */

static gboolean zoom_widget_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer zoom_data);

/* End Callback Declarations */

static GtkWidget *zoom_widget_create()
{
	zoom_win = gtk_drawing_area_new();
	gtk_widget_set_size_request(zoom_win,100,100);

	g_signal_connect(G_OBJECT(zoom_win),"expose-event",G_CALLBACK(zoom_widget_expose),NULL);

	return zoom_win;
}

GtkWidget *zoom_widget_get_widget()
{
	return zoom_widget_create();
}

void zoom_widget_set_pos(int xp, int yp)
{
	x = xp;
	y = yp;
	gtk_widget_queue_draw(zoom_win);
}

/* Callback Declarations */

gboolean zoom_widget_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer zoom_data)
{

	GdkPixmap *canv_buf = draw_get_backbuffer();
	int w, h;
	gdk_drawable_get_size(canv_buf, &w, &h);
	if(x>w-6) x = w-6;
	if(y>h-6) y = h-6;
	if(x<5) x = 5;
	if(y<5) y = 5;
	GdkGC *canv_gc = gdk_gc_new(canvas->window);
	GdkPixbuf *section = gdk_pixbuf_get_from_drawable(NULL ,canv_buf,NULL,x-5,y-5,0,0, 11,11);
	GdkPixbuf *section2 = gdk_pixbuf_scale_simple(section, 100, 100, GDK_INTERP_NEAREST);
	g_object_unref(section);
	gdk_draw_pixbuf(canvas->window,canv_gc,section2, 0, 0, 0, 0, 100, 100, GDK_RGB_DITHER_NONE, 0, 0);
	gdk_draw_rectangle(canvas->window, canv_gc, FALSE, 0,0, 100, 100);
	gdk_gc_set_function(canv_gc, GDK_INVERT);
	gdk_draw_rectangle(canvas->window, canv_gc, FALSE, 45,45, 10, 10);
	g_object_unref(canv_gc);
	g_object_unref(section2);
	
	return TRUE;
}
