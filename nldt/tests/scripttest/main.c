#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>

#include "sound.h"
#include "gtk_binds.h"
#include "gizmos.h"
#include "loader.h"

GTimer *giz_time;

static GtkWidget *fixed = NULL;

extern void main_window_add_child(GtkWidget *child, int x, int y)
{
	gtk_fixed_put(GTK_FIXED(fixed),child, x, y);
	gtk_widget_show_all(child);
}

static gboolean window_close(GtkWidget *window, gpointer user_data)
{
	nldt_gizmo_shutdown();
	sound_deinit();
	gtk_exit(0);
	return FALSE;
}

static gboolean update_gizmos(gpointer user_data)
{
	nldt_gizmo_update(g_timer_elapsed(giz_time,NULL));
	return TRUE;
}

static gboolean window_key_down(GtkWidget *canvas, GdkEventKey *ev, gpointer user_data)
{
	nldt_gizmo_key_down(ev->keyval);
	return TRUE;
}

static gboolean window_key_up(GtkWidget *canvas, GdkEventKey *ev, gpointer user_data)
{
	nldt_gizmo_key_up(ev->keyval);
	return TRUE;
}


int main(int argc, char **argv)
{
	g_set_application_name("NLDT: Factory");

	/*
		gtk_init merely handles the arguements passed
		to the program so you can change some gtk
		specific functions before our app gets to 
		read any arguements we may want to parse
	*/
	gtk_init(&argc, &argv);
	GtkWidget *window;
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window,600,500);
	
	fixed = gtk_fixed_new();
	
	gtk_container_add(GTK_CONTAINER(window),fixed);
	
	/*
		gtk_main is where we say good bye to our program
		flow and watch as it leaves home to become a man
	*/
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(window_close),NULL);
	g_signal_connect(G_OBJECT(window),"key-press-event",G_CALLBACK(window_key_down),NULL);
	g_signal_connect(G_OBJECT(window),"key-release-event",G_CALLBACK(window_key_up),NULL);
	gtk_widget_show_all(window);
	sound_init();
	if(argc == 2)
	{
		nldt_gizmo_load(argv[1]);
	}
	else
	{
		loader_show();	
	}
	giz_time = g_timer_new();
	g_timer_start(giz_time);	
	g_timeout_add(40,(GSourceFunc)update_gizmos,NULL);
	
	gtk_main();
	return 0;
}
