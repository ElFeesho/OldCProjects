#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
static gboolean drawing_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer user_data)
{
	GdkPixbuf *the_svg =rsvg_pixbuf_from_file_at_size("gearflowers.svg",canvas->allocation.width, canvas->allocation.height,NULL);
	gdk_draw_pixbuf(canvas->window,gdk_gc_new(canvas->window),the_svg,0,0,0,0,canvas->allocation.width,canvas->allocation.height,GDK_RGB_DITHER_NONE,0,0);
	return 1;
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *drawing = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(drawing),500,500);
	gtk_container_add(GTK_CONTAINER(window),drawing);
	gtk_widget_show_all(window);
	g_signal_connect(G_OBJECT(drawing),"expose-event",G_CALLBACK(drawing_expose),NULL);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_exit),NULL);
	gtk_main();
	return 0;
}
