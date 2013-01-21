#include <gtk/gtk.h>

#include "draw.h"

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *table = gtk_table_new(2,2,0);
	gtk_container_add(GTK_CONTAINER(window), table);

	gtk_table_attach_defaults(GTK_TABLE(table),canvas_create(), 1,2,0,1);

	g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}
