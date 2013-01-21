#include <gtk/gtk.h>
#include "network.h"
#include "mwin.h"
#include "globals.h"

/*
	Callback prototypes
*/

static gboolean mwin_window_delete(GtkWidget *window, gpointer user_data);
static void mwin_disp_name_activate(GtkWidget *dispname, gpointer user_data);

static GtkWidget *disp_name = NULL;
static GtkWidget *display_pic = NULL;

void mwin_change_nick(char *new_nick)
{
	if(globals_nick)
	{
		g_free(globals_nick);
		globals_nick = g_strdup(new_nick);
	}

	if(disp_name)
	{
		gtk_entry_set_text(GTK_ENTRY(disp_name),new_nick);
	}
}

void mwin_create(void)
{
	/* 
		This is going to be the main window the user will see.
		It's going to contain the buddy list and their screen
		name and personal message.
	*/
	static GtkWidget *window = NULL;
	if(window!=NULL)
	{
		gtk_widget_show_all(window);
		return;
	}
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "fMSN Buddy List");

	GtkWidget *vbox = gtk_vbox_new(0,0);
	
		GtkWidget *hbox = gtk_hbox_new(0,0);
	
			display_pic = gtk_image_new();
			gtk_widget_set_size_request(display_pic,96,96);

			disp_name = gtk_entry_new_with_max_length(32);
			
		gtk_box_pack_start(GTK_BOX(hbox),display_pic,1,1,4);
		gtk_box_pack_start(GTK_BOX(hbox),disp_name,1,0,0);

	gtk_box_pack_start(GTK_BOX(vbox),hbox,1,1,0);

		GtkWidget *contact_scroll = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(contact_scroll), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	
			GtkWidget *contact_list = gtk_tree_view_new();
		
		gtk_container_add(GTK_CONTAINER(contact_scroll),contact_list);
	
	gtk_box_pack_start(GTK_BOX(vbox),contact_scroll,1,1,0);

	gtk_container_add(GTK_CONTAINER(window), vbox);
	
	/* Hookup callbacks */
	g_signal_connect(G_OBJECT(window),"delete-event", G_CALLBACK(mwin_window_delete), NULL);
	g_signal_connect(G_OBJECT(disp_name), "activate", G_CALLBACK(mwin_disp_name_activate), NULL);
	gtk_widget_show_all(window);
}

/*
	Callback implementations
*/

gboolean mwin_window_delete(GtkWidget *window, gpointer user_data)
{
	/* TODO: Check whether the connection is established or not? */
	gtk_widget_hide(window);
	net_close();
	gtk_main_quit();
	return TRUE;
}

void mwin_disp_name_activate(GtkWidget *dispname, gpointer user_data)
{
	char *nick = (char*)gtk_entry_get_text(GTK_ENTRY(dispname));
	if(strlen(nick))
	{
		char *escp_nick = g_malloc(strlen(nick)*3+1);
		MSN_url_encode(nick, escp_nick, strlen(nick)*3+1);
		if(globals_nick)
			g_free(globals_nick);
		globals_nick = g_strdup(escp_nick);
		g_free(escp_nick);
		net_send_nick(globals_nick);
	}
	else
		gtk_entry_set_text(GTK_ENTRY(dispname),globals_nick);
}
