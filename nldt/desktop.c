#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "desktop.h"

static gboolean desktop_destroy(GtkWidget *desktop, gpointer user_data)
{
	//TODO: Make sure to check whether they REALLY wanna quit
	
	gtk_exit(0);
	return FALSE;
}

static gboolean desktop_keypress(GtkWidget *desktop, GdkEventKey *event, gpointer user_data)
{
	if((event->keyval&GDK_Escape)==GDK_Escape)
		desktop_destroy(desktop,NULL);
		
	return FALSE;
}
																		  
static GtkWidget *desktop_create()
{
	GtkWidget *desktop = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *fixed_pos_viewport = gtk_viewport_new(NULL,NULL);
	GtkWidget *fixed_pos = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(fixed_pos_viewport),fixed_pos);
	gtk_container_add(GTK_CONTAINER(desktop),fixed_pos_viewport);
	g_signal_connect(G_OBJECT(desktop),"delete-event",G_CALLBACK(desktop_destroy),NULL);
	g_signal_connect(G_OBJECT(desktop),"key-press-event",G_CALLBACK(desktop_keypress),NULL);
	
	gtk_window_maximize(GTK_WINDOW(desktop));
	gtk_window_fullscreen(GTK_WINDOW(desktop));
	return desktop;
}

extern void desktop_show()
{
	gtk_widget_show_all(desktop_create());
}
