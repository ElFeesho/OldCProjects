#include <gtk/gtk.h>
#include "window.h"
#include "groups.h"

/* Callback Prototypes */
static gboolean dbtool_window_delete(GtkWidget *window, gpointer user_data);

/* Helper Function Prototypes */
static GtkWidget *dbtool_window_new();

static GtkWidget *dbtool_window_new()
{
	GtkWidget *window, *vbox, *notebook;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	vbox = gtk_vbox_new(0,FALSE);
	
	notebook = gtk_notebook_new();
	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), groups_create(),gtk_label_new("Groups"));
	
	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
	
	gtk_container_add(GTK_CONTAINER(window),vbox);
	
	/* Callbacks */
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(dbtool_window_delete),NULL);

	return window;
}

extern void dbtool_window_create()
{
   GtkWidget *window = dbtool_window_new();
   gtk_widget_show_all(window);
}

/* Callback Definitions */
static gboolean dbtool_window_delete(GtkWidget *window, gpointer user_data)
{
	/*
		TODO: Add a dialog for "Do you really want to quit?"
	*/
	gtk_widget_destroy(window);
	gtk_exit(0);
	
	return FALSE;
}
