/*
 * 	First real window that is seen!
 */
#include <stdlib.h>
#include "database.h"
#include "board.h"
#include "misc.h"

/* Callbacks */

static gboolean browser_window_close(GtkWidget *window, gpointer user_data);
static void browser_tree_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);

/* End of Callbacks */

static GtkWidget *browser_create_treeview(const char *title, GtkListStore *store)
{
	GtkWidget *treeView = gtk_tree_view_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeView),0, title, gtk_cell_renderer_text_new(),"text",1,NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView),gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(store)));
	return treeView;
}

void browser_show()
{
	/* Main Window */
	GtkWidget *window;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"Powermaths");	
	gtk_widget_set_size_request(window,600,400);
	
	/* Layout */
	GtkWidget *table;
	GtkWidget *lvlScroll, *clsScroll, *lsnScroll;

	table = gtk_table_new(8,8,0);
	gtk_table_set_row_spacings(GTK_TABLE(table),4);
	gtk_table_set_col_spacings(GTK_TABLE(table),4);

	lvlScroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(lvlScroll),GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(lvlScroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);

	clsScroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(clsScroll),GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(clsScroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);

	lsnScroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(lsnScroll),GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(lsnScroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);

	/* UI */
	GtkWidget *level_view, *class_view, *lesson_view;
	GtkWidget *start, *exit;

	//Initialise Lists before creating tree views
	database_initialise_lists();

	//Fill the list
	database_fill_list(database_lvl1,"level_tbl");

	//Create tree views
	level_view = browser_create_treeview("Levels",database_lvl1);
	class_view = browser_create_treeview("Classes",database_lvl2);
	lesson_view = browser_create_treeview("Lessons",database_lvl3);


	start = gtk_button_new_with_label("Start");
	exit = gtk_button_new_with_label("Exit");
	
	/* Pack widgets into the table */

	gtk_container_add(GTK_CONTAINER(window),table); //Put the table in the window first
	
	gtk_container_add(GTK_CONTAINER(lvlScroll),level_view);
	gtk_container_add(GTK_CONTAINER(clsScroll),class_view);
	gtk_container_add(GTK_CONTAINER(lsnScroll),lesson_view);
		
	gtk_table_attach(GTK_TABLE(table),misc_create_banner("Welcome to Powermaths, please select a level, then a class and finally a lesson.",16000),2,7,1,2,GTK_EXPAND|GTK_FILL,0,0,0);
	gtk_table_attach_defaults(GTK_TABLE(table),lvlScroll,2,3,3,4);
	gtk_table_attach_defaults(GTK_TABLE(table),clsScroll,4,5,3,4);
	gtk_table_attach_defaults(GTK_TABLE(table),lsnScroll,6,7,3,4);
	gtk_table_attach(GTK_TABLE(table),start, 2,4,5,6,GTK_EXPAND|GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),exit, 5,7,5,6,GTK_EXPAND|GTK_FILL,0,0,0);


	/* Hookup callbacks */
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(browser_window_close),NULL);
	
	g_signal_connect(G_OBJECT(exit),"clicked",G_CALLBACK(browser_window_close),NULL);

	g_signal_connect(G_OBJECT(level_view),"row-activated",G_CALLBACK(browser_tree_view_row_activated),database_lvl2);
	g_signal_connect(G_OBJECT(class_view),"row-activated",G_CALLBACK(browser_tree_view_row_activated),database_lvl3);
	g_signal_connect(G_OBJECT(lesson_view),"row-activated",G_CALLBACK(browser_tree_view_row_activated),window);

	gtk_widget_show_all(window);
}

static gboolean browser_window_close(GtkWidget *window, gpointer user_data)
{
	gtk_exit(0);
	return 0;
}

static void browser_tree_view_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	GtkTreeIter iter;
	gtk_tree_model_get_iter(gtk_tree_view_get_model(tree_view),&iter, path);
	if(user_data == database_lvl2 || user_data == database_lvl3) //It's the level or class view
	{
		char *table;
		gtk_tree_model_get(gtk_tree_view_get_model(tree_view),&iter,2,&table,-1);
		if(user_data==database_lvl2)
			gtk_list_store_clear(database_lvl3);
		database_fill_list(user_data,table);
	}
	else
	{
		char *table, *name;
		gtk_tree_model_get(gtk_tree_view_get_model(tree_view),&iter,1,&name,2,&table,-1);
		board_show(name,table);
		gtk_widget_destroy(user_data);
	}
}
