#include <gtk/gtk.h>
#include "colour.h"

static GtkWidget *colour_canv;

GtkWidget *hue, *sat, *value;


static gboolean slider_update(GtkRange *range, GtkScrollType scroll, gdouble vlue, gpointer user_data) 
{
	double r, g, b;
	
	double h, s, v;
	
	h = gtk_range_get_value(GTK_RANGE(hue));
	s = gtk_range_get_value(GTK_RANGE(sat));
	v = gtk_range_get_value(GTK_RANGE(value));
	printf("H: %f S: %f V: %f\n",h,s,v);
	colour_convert(h, s, v, &r, &g, &b);
	
	cairo_t *cr = gdk_cairo_create(colour_canv->window);
	cairo_set_source_rgb(cr,r/255.0,g/255.0,b/255.0);
	cairo_rectangle(cr,0,0,colour_canv->allocation.width,colour_canv->allocation.height);
	cairo_fill(cr);
	cairo_destroy(cr);
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
	GtkWidget *hbox = gtk_hbox_new(FALSE,0);
	colour_canv = gtk_drawing_area_new();
	gtk_box_pack_start(GTK_BOX(hbox),colour_canv,TRUE,TRUE,0);
	hue = gtk_vscale_new_with_range(0,360,0.1);
	sat = gtk_vscale_new_with_range(0,1,0.1);
	value = gtk_vscale_new_with_range(0,1,0.1);
	gtk_box_pack_start(GTK_BOX(hbox),hue,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbox),sat,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbox),value,TRUE,TRUE,0);
	gtk_container_add(GTK_CONTAINER(window),hbox);
	gtk_widget_show_all(window);
	g_signal_connect(G_OBJECT(hue),"change-value", G_CALLBACK(slider_update),NULL);
	g_signal_connect(G_OBJECT(sat),"change-value", G_CALLBACK(slider_update),NULL);
	g_signal_connect(G_OBJECT(value),"change-value", G_CALLBACK(slider_update),NULL);
	g_signal_connect(G_OBJECT(window),"delete-event", G_CALLBACK(win_delete),NULL);
	gtk_main();
	return 0;
}
