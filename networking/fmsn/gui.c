#include <gtk/gtk.h>
#include <gnet.h>
#include "network.h"
#include "gui.h"
#include "mwin.h"

/* Some local globals */
static GtkWidget *email_entry, *password_entry;

/* Prototypes for callbacks */
static gboolean gui_main_window_delete(GtkWidget *window, gpointer user_data);
static void gui_tray_activate(GtkWidget *tray_icon, gpointer user_data);
static void gui_login_clicked(GtkWidget *login, gpointer user_data);
static void gui_quit_clicked(GtkWidget *login, gpointer user_data);

void gui_init(void)
{
	GtkStatusIcon *tray_icon = gtk_status_icon_new_from_file("tray.png");
	
	GtkWidget *window, *vbox, *table, *login, *quit, *banner;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	/* Format window to make it look pretty */
	gtk_window_set_title(GTK_WINDOW(window), "fMSN: Login"); 
	gtk_container_set_border_width(GTK_CONTAINER(window), 3);
	
	banner = gtk_image_new_from_file("banner.png");
	
	email_entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(email_entry),32);
	
	password_entry = gtk_entry_new();
	/* Visibility is whether the characters should be visible, not the textbox itself */
	gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
	
	login = gtk_button_new_with_label("Login");
	quit = gtk_button_new_with_label("Quit");
	
	/* New table with 2 rows, 3 columns and not homogenous */	
	table = gtk_table_new(2, 3, FALSE);
	gtk_table_set_col_spacings(GTK_TABLE(table),4);
	gtk_table_set_row_spacings(GTK_TABLE(table),4);
	
	/* 
		Time to push things into our table: table attachment is a bit confusing
		at first, but after time, you'll learn to hate it like me :D...
		
		gtk_table_attach(TABLE_WE_WANT_TO_PUT_STUFF_INTO, WIDGET_WE_ARE_PUTTING_IN, left_pos, right_pos, top_pos, bottom_pos, H_ATTRIBUTES, V_ATTRIBUTES, ROW_PADDING, COL_PADDING);
		refer to the API reference at www.gtk.org for more info on the _real_ function sig.
	*/
	gtk_table_attach(GTK_TABLE(table), gtk_label_new("E-Mail:"),0,1,0,1,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table), gtk_label_new("Password:"),0,1,1,2,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table), email_entry,1,2,0,1,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table), password_entry,1,2,1,2,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table), login,2,3,0,1,GTK_FILL|GTK_EXPAND,0,0,0);
	gtk_table_attach(GTK_TABLE(table), quit,2,3,1,2,GTK_FILL|GTK_EXPAND,0,0,0);
	
	/* Now put the banner and the table into the vbox */
	vbox = gtk_vbox_new(FALSE, 4);
	gtk_box_pack_start(GTK_BOX(vbox), banner, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 0);
	
	/* Now push the vbox into the window so everything is displayed! */
	gtk_container_add(GTK_CONTAINER(window), vbox);
	
	/* Hookup callbacks */
	g_signal_connect(G_OBJECT(window),"delete-event", G_CALLBACK(gui_main_window_delete), NULL);
	g_signal_connect(G_OBJECT(tray_icon), "activate", G_CALLBACK(gui_tray_activate), NULL);
	g_signal_connect(G_OBJECT(login), "clicked", G_CALLBACK(gui_login_clicked), window);
	g_signal_connect(G_OBJECT(quit), "clicked", G_CALLBACK(gui_quit_clicked), NULL);
	/* If user hits enter after entering password, it will try to login */
	g_signal_connect(G_OBJECT(password_entry), "activate", G_CALLBACK(gui_login_clicked), window);
	
	gtk_widget_show_all(window);
}

/* Callback definitions */

static gboolean gui_main_window_delete(GtkWidget *window, gpointer user_data)
{
	/*
		TODO: Check for active connection etc
	*/
	gtk_main_quit();
	return FALSE;
}

static void gui_tray_activate(GtkWidget *tray_icon, gpointer user_data)
{
	if(net_connected())
		mwin_create();
}

static void gui_login_clicked(GtkWidget *login, gpointer user_data)
{
	gtk_widget_hide(GTK_WIDGET(user_data));
	net_login((char*)gtk_entry_get_text(GTK_ENTRY(email_entry)), (char*)gtk_entry_get_text(GTK_ENTRY(password_entry)));
	gtk_widget_destroy(GTK_WIDGET(user_data));
}

static void gui_quit_clicked(GtkWidget *login, gpointer user_data)
{
	gtk_main_quit();
}
