#include <gtk/gtk.h>
#include "banner.h"

/* Callbacks */
static gboolean client_info_destroy(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	return FALSE;
}

void client_info_show(char *nick, char *name, char *website, char *email)
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	GtkWidget *table = gtk_table_new(2,5,0);
	GtkWidget *alignment = gtk_alignment_new(1,0,0,0);
	GtkWidget *button = gtk_button_new_with_label("OK");
	gtk_container_add(GTK_CONTAINER(alignment),button);
	gtk_table_attach_defaults(GTK_TABLE(table),banner_create(nick,16),0,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Name: "),0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new(name),1,2,1,2); 
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Website: "),0,1,2,3);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new(website),1,2,2,3);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Email: "),0,1,3,4);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new(email),1,2,3,4);
	gtk_table_attach_defaults(GTK_TABLE(table),alignment,0,2,4,5);
	gtk_container_add(GTK_CONTAINER(window),table);
	
	g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(client_info_destroy), NULL);
	g_signal_connect_swapped(G_OBJECT(button), "clicked", G_CALLBACK(client_info_destroy), window);
	gtk_widget_show_all(window);
}
