/*
 * 	First real window that is seen!
 */
#include <gtk/gtk.h>
#include <stdlib.h>
#include "database.h"
#include "board.h"
#include "misc.h"
#include "levels.h"
#include "groups.h"
#include "gtkpmbutton.h"
#include "gtkpmhbutton.h"
#include "gtkvimage.h"
#include "search.h"

/* Callbacks */

static gboolean browser_window_close(GtkWidget *window, gpointer user_data);
static void browser_home_clicked(GtkWidget *home, gpointer data);
/* End of Callbacks */

static GtkWidget *browser_viewport = NULL, *vbox = NULL;

GtkWidget *browser_table = NULL;
static GtkWidget *browser_scroll_hbox = NULL;

static void browser_history_button_click(GtkWidget *button, void *(table_create)())
{
	gtk_container_forall(GTK_CONTAINER(browser_table),(GtkCallback)gtk_widget_destroy,NULL);
	table_create();
	GList *hbox_children = gtk_container_get_children(GTK_CONTAINER(browser_scroll_hbox));
	int found = 0;
	while(hbox_children!=NULL)
	{
		if(hbox_children->data == button)
		{
			gtk_widget_destroy(button);	
			found = 1;
		}
		if(found)
			gtk_widget_destroy(GTK_WIDGET(hbox_children->data));
		hbox_children = hbox_children->next;
	}

}

void browser_add_history(const char *text, void *(table_create)())
{
	GtkWidget *button = gtk_pmhbutton_new((char*)text);
	static int height = -1;
	if(height==-1)
		height = gdk_screen_get_height(gdk_screen_get_default());
	gtk_widget_set_size_request(button,height/7,height/7);
	gtk_box_pack_start(GTK_BOX(browser_scroll_hbox),button,0,0,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(browser_history_button_click),table_create);
	gtk_widget_show(button);
}

void browser_show()
{
	/* Main Window */
	GtkWidget *window, *scroller, *scroller_vp, *header_hbox, *home, *search, *exit_button;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"PowerMaths");
	//gtk_window_set_decorated(GTK_WINDOW(window),FALSE);
	//gtk_window_maximize(GTK_WINDOW(window));
	gtk_widget_set_size_request(window,gdk_screen_get_width(gdk_screen_get_default()),gdk_screen_get_height(gdk_screen_get_default()));
	gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
	gtk_window_fullscreen(GTK_WINDOW(window));
	
	scroller = gtk_hbox_new(0,5);
	scroller_vp = gtk_viewport_new(NULL,NULL);
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(scroller_vp),GTK_SHADOW_NONE);
	gtk_container_add(GTK_CONTAINER(scroller_vp),scroller);
	
	browser_viewport = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(browser_viewport),GTK_SHADOW_NONE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(browser_viewport),GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);
	int height = gdk_screen_get_height(gdk_screen_get_default());
	browser_scroll_hbox = gtk_hbox_new(0,5);
	header_hbox = gtk_hbox_new(0,0);
	home = gtk_pmhbutton_new("Home");
	search = gtk_pmhbutton_new("Search");
	gtk_widget_set_size_request(home,height/7,height/7);
	gtk_widget_set_size_request(search,height/7,height/7);
	gtk_widget_set_size_request(browser_scroll_hbox,0,height/7);
	gtk_widget_set_size_request(header_hbox,0,height/7);
	
	gtk_box_pack_start(GTK_BOX(scroller),home,0,0,0);
	gtk_box_pack_start(GTK_BOX(scroller),browser_scroll_hbox,1,1,0);
	gtk_box_pack_start(GTK_BOX(scroller),search,0,0,0);
	
	vbox = gtk_vbox_new(0,0);
	groups_create_table();
	misc_set_background(scroller_vp,42405,0,8481);
	misc_set_background(header_hbox,42405,0,8481);
	
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(browser_viewport),browser_table);
	GtkWidget *logo = gtk_vimage_new("pmlogo.svg");
	gtk_widget_set_size_request(logo,height/7,height/7);
	gtk_box_pack_start(GTK_BOX(header_hbox),logo,0,1,0);
	gtk_box_pack_start(GTK_BOX(header_hbox),misc_create_banner("PowerMaths",80),1,1,0);
	exit_button = gtk_pmhbutton_new("X");
	gtk_widget_set_size_request(exit_button,height/7,height/7);
	gtk_box_pack_end(GTK_BOX(header_hbox),exit_button,0,1,0);
	
	
	gtk_box_pack_start(GTK_BOX(vbox),header_hbox,0,0,0);
	gtk_box_pack_start(GTK_BOX(vbox),browser_viewport,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),scroller_vp,0,0,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(browser_window_close),NULL);	
	g_signal_connect(G_OBJECT(home),"clicked",G_CALLBACK(browser_home_clicked),NULL);
	g_signal_connect(G_OBJECT(search),"clicked",G_CALLBACK(search_create_table),NULL);
	g_signal_connect(G_OBJECT(exit_button),"clicked",G_CALLBACK(gtk_exit),NULL);
	gtk_widget_show_all(window);
}

static gboolean browser_window_close(GtkWidget *window, gpointer user_data)
{
	gtk_exit(0);
	return 0;
}

void browser_home_clicked(GtkWidget *home, gpointer data)
{
	gtk_container_forall(GTK_CONTAINER(browser_table),(GtkCallback)gtk_widget_destroy,NULL);
	gtk_container_forall(GTK_CONTAINER(browser_scroll_hbox),(GtkCallback)gtk_widget_destroy,NULL);
	groups_create_table();
}
