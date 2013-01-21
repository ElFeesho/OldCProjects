#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "chat.h"
#include "error.h"
#include "network.h"
#include "banner.h"

/* Callback Declarations */

static gboolean chan_diag_window_delete(GtkWidget *window, gpointer user_data);
static void chan_diag_ok_clicked(GtkWidget *window, gpointer user_data);
static void chan_diag_cancel_clicked(GtkWidget *window, gpointer user_data);
static gboolean chan_diag_channel_activate(GtkWidget *window, gpointer user_data);

/* End of Declarations */

static GtkWidget *channel = NULL;

static GtkWidget *chan_diag_init()
{
	GtkWidget *window, *table, *ok, *cancel;
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint(GTK_WINDOW(window),GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_resizable(GTK_WINDOW(window),0);
	gtk_window_set_title(GTK_WINDOW(window),"Change Channel");
	
	table = gtk_table_new(3,3,0);
	gtk_container_add(GTK_CONTAINER(window),table);

	channel = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(channel),16);
	gtk_entry_set_text(GTK_ENTRY(channel),globals_channel);
	ok = gtk_button_new_with_label("Ok");
	cancel = gtk_button_new_with_label("Cancel");

	gtk_table_attach_defaults(GTK_TABLE(table),banner_create("Change Channel",23),0,3,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Channel:"),0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(table),channel,1,2,1,2);
	gtk_table_attach_defaults(GTK_TABLE(table),ok,0,1,2,3);
	gtk_table_attach_defaults(GTK_TABLE(table),cancel,1,2,2,3);

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(chan_diag_window_delete),NULL);	
	g_signal_connect_swapped(G_OBJECT(ok),"clicked",G_CALLBACK(chan_diag_ok_clicked),window);
	g_signal_connect_swapped(G_OBJECT(cancel),"clicked",G_CALLBACK(chan_diag_cancel_clicked),window);
	g_signal_connect_swapped(G_OBJECT(channel),"activate",G_CALLBACK(chan_diag_channel_activate),window);
	return window;
}

void chan_diag_show()
{
	gtk_widget_show_all(chan_diag_init());
}

/* Callback definitions */

gboolean chan_diag_window_delete(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	return 0;
}

void chan_diag_ok_clicked(GtkWidget *window, gpointer user_data)
{
	char *channel_text = (char*)gtk_entry_get_text(GTK_ENTRY(channel));
	if(strcmp(globals_channel,channel_text)!=0)
	{
		if(channel_text[0]=='#')
		{
			if(strlen(channel_text)>1)
			{
				strcpy(globals_channel,gtk_entry_get_text(GTK_ENTRY(channel)));
				char channel_changed[128] = { 0 };
				sprintf(channel_changed,"*** Changed channel to %s\n",channel_text);
				chat_append(channel_changed);
				network_send_channel();
			}
			else
			{
				error_dialog("Please enter a channel!");
				return;
			}
		}
		else
		{
			error_dialog("Channels must begin with a #!");
			return;
		}
	}
	gtk_widget_destroy(window);
}

void chan_diag_cancel_clicked(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
}

gboolean chan_diag_channel_activate(GtkWidget *window, gpointer user_data)
{
	chan_diag_ok_clicked(window, user_data);
	return 1;
}

/* End of definitions */
