#include <gtk/gtk.h>
#include "colour.h"

static GtkWidget *colour_canv;
static GtkWidget *value_canv;

static gboolean colour_canv_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer user_data)
{
	int i = 0;
	int k = 0;
	double sat = 1.0;
	double val = 1.0;
	double hue = 0.0;
	for(k = 0;k<50;k++)
	{
		for (i = 0;i<100;i++)
		{
			hue = i/100.0*360.0;
			GdkGC *gc = gdk_gc_new(colour_canv->window);
			GdkColor col;
			double r, g, b;
			colour_convert(hue,sat,val, &r, &g, &b);
			col.red = r*255;
			col.green = g*255;
			col.blue = b*255;
			gdk_gc_set_rgb_fg_color(gc, &col);
			gdk_draw_line (colour_canv->window, gc,i,k,i,k);
		}
		sat -= 1.0/50.0;
	}
	return FALSE;
}

static gboolean colour_canv_click(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	double hue = ev->x/100*360.0;
	double sat = 1.0 - (ev->y*1.0/50.0);
	double val = 1.0;
	int i = 0;
	for(i = 0;i<50;i++)
	{
		GdkGC *gc = gdk_gc_new(value_canv->window);
		GdkColor col;
		double r, g, b;
		colour_convert(hue,sat,val, &r, &g, &b);
		col.red = r*255;
		col.green = g*255;
		col.blue = b*255;
		gdk_gc_set_rgb_fg_color(gc, &col);
		gdk_draw_line (value_canv->window, gc,0,i,20,i);
		val-=1.0/50.0;
	}
	return FALSE;
}

static gboolean colour_canv_move(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data)
{
	if(ev->x>100)
		ev->x = 100;
	if(ev->x<0)
		ev->x = 0;
	if(ev->y>50)
		ev->y = 50;
	if(ev->y<0)
		ev->y = 0;
	double hue = ev->x/100*360.0;
	double sat = 1.0 - (ev->y*1.0/50.0);
	double val = 1.0;
	int i = 0;
	for(i = 0;i<50;i++)
	{
		GdkGC *gc = gdk_gc_new(value_canv->window);
		GdkColor col;
		double r, g, b;
		colour_convert(hue,sat,val, &r, &g, &b);
		col.red = r*255;
		col.green = g*255;
		col.blue = b*255;
		gdk_gc_set_rgb_fg_color(gc, &col);
		gdk_draw_line (value_canv->window, gc,0,i,20,i);
		val-=1.0/50.0;
	}
	return FALSE;
}

static gboolean win_delete(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	gtk_main_quit();
	return FALSE;
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *hbox = gtk_hbox_new(TRUE,0);
	colour_canv = gtk_drawing_area_new();
	gtk_widget_add_events(colour_canv,GDK_BUTTON_PRESS_MASK|GDK_BUTTON_MOTION_MASK);
	value_canv = gtk_drawing_area_new();
	gtk_widget_set_size_request(colour_canv,100,50);
	gtk_widget_set_size_request(colour_canv,20,50);
	gtk_box_pack_start(GTK_BOX(hbox),colour_canv,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbox),value_canv,FALSE,TRUE,0);
	gtk_container_add(GTK_CONTAINER(window),hbox);
	gtk_widget_show_all(window);
	g_signal_connect(G_OBJECT(colour_canv),"expose-event", G_CALLBACK(colour_canv_expose), NULL);
	g_signal_connect(G_OBJECT(colour_canv),"button-press-event", G_CALLBACK(colour_canv_click), NULL);
	g_signal_connect(G_OBJECT(colour_canv),"motion-notify-event", G_CALLBACK(colour_canv_move), NULL);
	g_signal_connect(G_OBJECT(window),"delete-event", G_CALLBACK(win_delete),NULL);
	gtk_main();
	return 0;
}
