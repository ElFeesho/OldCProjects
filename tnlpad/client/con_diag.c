#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "network.h"

/* Callback Declarations */

static gboolean con_diag_delete(GtkWidget *window, gpointer user_data);
static void con_diag_connect_clicked(GtkWidget *connect, gpointer user_data);

static GtkWidget *server, *port;

/* End of Callbacks */

GtkWidget *con_diag_init()
{
	GtkWidget *window, *table, *banner, *connect, *cancel;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(window),0);
	gtk_window_set_type_hint(GTK_WINDOW(window),GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_title(GTK_WINDOW(window),"Connect to a Tnl Pad Server");
	
	table = gtk_table_new(5,2,1);
	gtk_table_set_row_spacings(GTK_TABLE(table),4);
	
	banner = gtk_image_new_from_file("pixbufs/banner.png");
	
	server = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(server),globals_server);
	port = gtk_entry_new();
	char ptext[6];
	sprintf(ptext,"%d",globals_port);
	gtk_entry_set_text(GTK_ENTRY(port),ptext);
	connect = gtk_button_new_with_label("Connect");
	cancel = gtk_button_new_with_label("Cancel");

	gtk_container_set_border_width(GTK_CONTAINER(window),3);
	gtk_table_attach_defaults(GTK_TABLE(table),banner,0,2,0,2);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Server:"),0,1,2,3);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Port:"),0,1,3,4);
	gtk_table_attach_defaults(GTK_TABLE(table),server,1,2,2,3);
	gtk_table_attach_defaults(GTK_TABLE(table),port,1,2,3,4);
	gtk_table_attach_defaults(GTK_TABLE(table),connect,0,1,4,5);
	gtk_table_attach_defaults(GTK_TABLE(table),cancel,1,2,4,5);
	gtk_container_add(GTK_CONTAINER(window),table);

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(con_diag_delete),NULL);
	g_signal_connect_swapped(G_OBJECT(connect),"clicked",G_CALLBACK(con_diag_connect_clicked),window);
	g_signal_connect_swapped(G_OBJECT(cancel),"clicked",G_CALLBACK(con_diag_delete),window);
	return window;
}

void con_diag_show(GtkWidget *toolbar_connect, gpointer user_data)
{
	if(!globals_connected)
		gtk_widget_show_all(con_diag_init());
	else
		network_disconnect(0);
}

/* Callback Definitions */

gboolean con_diag_delete(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	return 0;
}

void con_diag_connect_clicked(GtkWidget *window, gpointer user_data)
{
	strcpy(globals_server,gtk_entry_get_text(GTK_ENTRY(server)));
	globals_port = atoi(gtk_entry_get_text(GTK_ENTRY(port)));
	network_connect();
	gtk_widget_destroy(window);		
}

/* End of Callbacks */
