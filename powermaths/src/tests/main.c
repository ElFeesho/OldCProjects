#include <gtk/gtk.h>

void draw_clicked(GtkWidget *button, GtkWidget *canvas)
{
	GdkRectangle rect = { 20,20,50,50 };
	GdkRegion *rect_region = gdk_region_rectangle(&rect);
	gdk_window_begin_paint_region(canvas->window,rect_region);
	cairo_t *cr = gdk_cairo_create(canvas->window);
	cairo_move_to(cr,0,0);
	cairo_line_to(cr,100,100);
	cairo_stroke(cr);
	cairo_destroy(cr);
	gdk_window_end_paint(canvas->window);
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *vbox = gtk_vbox_new(0,0);
	GtkWidget *drw_area = gtk_drawing_area_new();
	GtkWidget *draw = gtk_button_new_with_label("Draw");

	gtk_box_pack_start(GTK_BOX(vbox),drw_area,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),draw,TRUE,TRUE,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	gtk_widget_show_all(window);

	gtk_widget_set_size_request(drw_area,100,100);

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_exit),NULL);
	g_signal_connect(G_OBJECT(draw),"clicked",G_CALLBACK(draw_clicked),drw_area);
	gtk_main();
	return 0;
}
