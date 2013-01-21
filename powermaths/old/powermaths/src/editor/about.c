#include <gtk/gtk.h>

extern GtkWidget *about_create()
{
	GtkWidget *window = NULL; 
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(window),0);
	
	return window;
}

void about_show()
{
	static GtkWidget *window = NULL;
	if(window == NULL)
		window = about_create();

	gtk_widget_show_all(window);
}
