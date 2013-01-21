#include <gtk/gtk.h>
#include <string.h>
#include "globals.h"
#include "error.h"

/*
	Callbacks
*/

static GtkWidget *server_host, *user_name, *user_website, *user_email;

static gboolean preferences_window_destroy(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);

	return FALSE;
}

static void preferences_store(GtkWidget *button, GtkWidget *window)
{
	char *new_server = (char*)gtk_entry_get_text(GTK_ENTRY(server_host));
	char *new_name = (char*)gtk_entry_get_text(GTK_ENTRY(user_name));
	char *new_website = (char*)gtk_entry_get_text(GTK_ENTRY(user_website));
	char *new_email = (char*)gtk_entry_get_text(GTK_ENTRY(user_email));
	
	strncpy(globals_server, new_server, 127);
	strncpy(globals_name, new_name, 31);
	strncpy(globals_website, new_website, 31);
	strncpy(globals_email, new_email, 31);
	
	gtk_widget_destroy(window);
}

void preferences_show()
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"Tnl Pad - Preferences");
	gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
	
	GtkWidget *vbox = gtk_vbox_new(FALSE, FALSE);
	
	GtkWidget *server_frame = gtk_frame_new("Server Details");
	
	GtkWidget *server_table = gtk_table_new(2,2,0);
	
	server_host = gtk_entry_new_with_max_length(128);
	if(globals_server!=NULL)
		gtk_entry_set_text(GTK_ENTRY(server_host),globals_server);
	
	/* Pack server table here */
	
	gtk_table_attach(GTK_TABLE(server_table),gtk_label_new("Server:"),0,1,0,1,1,0,0,0);
	gtk_table_attach(GTK_TABLE(server_table),server_host,1,2,0,1,0,0,0,0);
	
	gtk_container_add(GTK_CONTAINER(server_frame), server_table);
	
	GtkWidget *user_frame = gtk_frame_new("User Details");
	
	GtkWidget *user_table = gtk_table_new(2,4,0);
	
	user_name = gtk_entry_new_with_max_length(32);
	if(globals_name!=NULL)
		gtk_entry_set_text(GTK_ENTRY(user_name),globals_name);
	user_website = gtk_entry_new_with_max_length(32);
	if(globals_website!=NULL)
		gtk_entry_set_text(GTK_ENTRY(user_website),globals_website);
	user_email = gtk_entry_new_with_max_length(32);
	if(globals_email!=NULL)
		gtk_entry_set_text(GTK_ENTRY(user_email),globals_email);
		
	gtk_table_attach(GTK_TABLE(user_table),gtk_label_new("Name: "),0,1,0,1,0,0,0,0);
	gtk_table_attach(GTK_TABLE(user_table),user_name,1,2,0,1,0,0,0,0);
	gtk_table_attach(GTK_TABLE(user_table),gtk_label_new("Website: "),0,1,1,2,0,0,0,0);
	gtk_table_attach(GTK_TABLE(user_table),user_website,1,2,1,2,0,0,0,0);
	gtk_table_attach(GTK_TABLE(user_table),gtk_label_new("E-Mail: "),0,1,2,3,0,0,0,0);
	gtk_table_attach(GTK_TABLE(user_table),user_email,1,2,2,3,0,0,0,0);
	
	gtk_container_add(GTK_CONTAINER(user_frame), user_table);
	
	GtkWidget *alignment = gtk_alignment_new(1,0,0,0);
	GtkWidget *hbox = gtk_hbox_new(TRUE,0);
	GtkWidget *ok_button = gtk_button_new_with_label("OK");
	GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
	
	gtk_box_pack_start(GTK_BOX(hbox),ok_button,0,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbox),cancel_button,0,TRUE,0);
	
	gtk_container_add(GTK_CONTAINER(alignment),hbox);
	
	gtk_box_pack_start(GTK_BOX(vbox),server_frame,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),user_frame,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),alignment,TRUE,TRUE,0);
	
	gtk_container_add(GTK_CONTAINER(window), vbox);
	
	g_signal_connect(G_OBJECT(window),"delete-event", G_CALLBACK(preferences_window_destroy), NULL);
	g_signal_connect_swapped(G_OBJECT(cancel_button),"clicked", G_CALLBACK(preferences_window_destroy), window);
	g_signal_connect(G_OBJECT(ok_button), "clicked", G_CALLBACK(preferences_store), window);	
	gtk_widget_show_all(window);
}
