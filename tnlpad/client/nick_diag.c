#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "network.h"
#include "client.h"
#include "chat.h"
#include "error.h"

/* Callback Declarations */

static gboolean nick_diag_window_delete(GtkWidget *window, gpointer user_data);
static void nick_diag_ok_clicked(GtkWidget *window, gpointer user_data);
static gboolean nick_diag_nick_activate(GtkWidget *window, gpointer user_data);

/* End of Declarations */

static GtkWidget *nick;

static GtkWidget *nick_diag_init()
{
	GtkWidget *window, *table, *ok, *cancel, *banner;
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint(GTK_WINDOW(window),GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_resizable(GTK_WINDOW(window),0);
	gtk_window_set_title(GTK_WINDOW(window),"Change your Nick");
	table = gtk_table_new(3,2,0);
	banner = gtk_image_new_from_file("pixbufs/banner.png");
	gtk_table_set_row_spacings(GTK_TABLE(table),4);
	nick = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(nick),globals_nick);
	gtk_entry_set_max_length(GTK_ENTRY(nick),16);
	ok = gtk_button_new_with_label("Ok");
	cancel = gtk_button_new_with_label("Cancel");
	gtk_container_set_border_width(GTK_CONTAINER(window),3);
	gtk_table_attach_defaults(GTK_TABLE(table),banner,0,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Nick:"),0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(table),nick,1,2,1,2);
	gtk_table_attach_defaults(GTK_TABLE(table),ok,0,1,2,3);
	gtk_table_attach_defaults(GTK_TABLE(table),cancel,1,2,2,3);
	gtk_container_add(GTK_CONTAINER(window),table);

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(nick_diag_window_delete),NULL);
	g_signal_connect_swapped(G_OBJECT(nick),"activate",G_CALLBACK(nick_diag_nick_activate),window);
	g_signal_connect_swapped(G_OBJECT(ok),"clicked",G_CALLBACK(nick_diag_ok_clicked),window);
	g_signal_connect_swapped(G_OBJECT(cancel),"clicked",G_CALLBACK(nick_diag_window_delete),window);
	return window;
}

int nick_change(char *nick_text)
{
	if(strcmp(globals_nick,nick_text)!=0)
	{
		if(strlen(nick_text)>1)
		{
			strncpy(globals_nick,nick_text,16);
			client_change(globals_nick,-1);
			network_send_nick();
		}
		else
		{
			error_dialog("Please enter a nick!");
			return 0;
		}
	}
	return 1;
}

void nick_diag_show()
{
	gtk_widget_show_all(nick_diag_init());
}

/* Callback Definitions */

gboolean nick_diag_window_delete(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	return 0;
}
void nick_diag_ok_clicked(GtkWidget *window, gpointer user_data)
{
	char *nick_text = (char*)gtk_entry_get_text(GTK_ENTRY(nick));
	if(nick_change(nick_text))
		gtk_widget_destroy(window);
}

gboolean nick_diag_nick_activate(GtkWidget *window, gpointer user_data)
{
	nick_diag_ok_clicked(window, user_data);
	return 1;	
}

/* End of Definitions */
