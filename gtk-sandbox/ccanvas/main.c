#include <gtk/gtk.h>
#include "gtkccanvas.h"

static void on_size_change(GtkWidget *slider, GtkWidget *ccanvas)
{
	gtk_ccanvas_set_brush_size(GTK_CCANVAS(ccanvas),(int)gtk_range_get_value(GTK_RANGE(slider)));
}

int main(int argc, char **argv)
{
	GtkWidget *window, *ccanvas, *vbox, *size_scroll;
   gtk_init(&argc, &argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window,300,200);
	ccanvas = gtk_ccanvas_new_with_border();
	
	vbox = gtk_vbox_new(0,0);
	
	size_scroll = gtk_hscale_new_with_range(1.0,20.0,1.0);
	
	gtk_box_pack_start(GTK_BOX(vbox),ccanvas,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),gtk_label_new("Brush size:"),FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),size_scroll,FALSE,TRUE,0);
	
	gtk_container_add(GTK_CONTAINER(window),vbox);
	
	g_signal_connect(G_OBJECT(size_scroll),"value-changed",G_CALLBACK(on_size_change),ccanvas);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_exit),NULL);
	
	gtk_widget_show_all(window);
		
   gtk_main();
   return 0;
}
