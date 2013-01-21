#include <gtk/gtk.h>
#include <stdio.h>
#include <stdint.h>
#include "globals.h"
#include "chat.h"
#include "client_info.h"
#include "network.h"

#ifdef LOG
#define CLIENT_LOG 0x40
void client_set_logged(int32_t id);
char client_get_logged(int32_t id);

static void client_toggle_log(GtkWidget *log_menu_item, gpointer id)
{
	/*
		Toggle Logging :D
	*/
	client_set_logged((long)id);
}
#endif

static void client_take_ink(GtkWidget *log_menu_item, gpointer id)
{
	network_send_take_ink(((char)((long)id)));
}

static void client_take_voice(GtkWidget *log_menu_item, gpointer id)
{
	network_send_take_voice(((char)((long)id)));
}

static void client_give_ink(GtkWidget *log_menu_item, gpointer id)
{
	network_send_give_ink(((char)((long)id)));
}

static void client_give_voice(GtkWidget *log_menu_item, gpointer id)
{
	network_send_give_voice(((char)((long)&id)));
}


static GtkWidget *client_tree_view = 0;
static GtkListStore *client_list = 0;

static gboolean client_popup_menu(GtkWidget *tree_view, GdkEventButton *event)
{
	/*
		This will show up a pop up menu with Ignore settings (and maybe something else ;))
	*/
	/*
		Make sure that a row is selected
	*/
	GtkTreeSelection *client_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	if(gtk_tree_selection_count_selected_rows(client_selection)==0)
		return FALSE;
		
	/*
		Get the row that is currently selected
	*/
	GList *row = gtk_tree_selection_get_selected_rows(client_selection, (GtkTreeModel**)(&client_list));
	GtkTreePath *client_tree_path = g_list_nth_data(row,0);
	if(!client_tree_path)
		return FALSE;
	
	/*
		Get an iterator reference to the row
	*/
	GtkTreeIter client_row_pos;
	gtk_tree_model_get_iter(GTK_TREE_MODEL(client_list), &client_row_pos, client_tree_path);
	
	/*
		Finally, extract some information from it
	*/
	char *nick;
	int id = 0;
	int stats = 0;
	gtk_tree_model_get(GTK_TREE_MODEL(client_list),&client_row_pos,0,&nick,1,&id,2, &stats,-1);
	GtkWidget *context_menu, *ignore_menu, *actions_menu;
	GtkWidget *menu_item;
	int button, event_time;
	
	ignore_menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_label("Ignore Voice");
	gtk_menu_append(GTK_MENU(ignore_menu),menu_item);
	menu_item = gtk_menu_item_new_with_label("Ignore Ink");
	gtk_menu_append(GTK_MENU(ignore_menu),menu_item);
	menu_item = gtk_menu_item_new_with_label("Ignore All");
	gtk_menu_append(GTK_MENU(ignore_menu),menu_item);
	actions_menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_label("Kick to #limbo");
	gtk_menu_append(GTK_MENU(actions_menu),menu_item);
	menu_item = gtk_menu_item_new_with_label("Ban off of the network");
	gtk_menu_append(GTK_MENU(actions_menu),menu_item);
	menu_item = gtk_menu_item_new_with_label("Take Voice");
	g_signal_connect(G_OBJECT(menu_item),"activate",G_CALLBACK(client_take_voice),(gpointer)id);
	gtk_menu_append(GTK_MENU(actions_menu),menu_item);
	menu_item = gtk_menu_item_new_with_label("Take Ink");
	g_signal_connect(G_OBJECT(menu_item),"activate",G_CALLBACK(client_take_ink),(gpointer)id);
	gtk_menu_append(GTK_MENU(actions_menu),menu_item);
	menu_item = gtk_menu_item_new_with_label("Give Voice");
	g_signal_connect(G_OBJECT(menu_item),"activate",G_CALLBACK(client_give_voice),(gpointer)id);
	gtk_menu_append(GTK_MENU(actions_menu),menu_item);
	menu_item = gtk_menu_item_new_with_label("Give Ink");
	g_signal_connect(G_OBJECT(menu_item),"activate",G_CALLBACK(client_give_ink),(gpointer)id);
	gtk_menu_append(GTK_MENU(actions_menu),menu_item);
	
	context_menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_label(nick);
	gtk_widget_set_sensitive(menu_item,FALSE);
	gtk_menu_append(GTK_MENU(context_menu),menu_item);
	menu_item = gtk_separator_menu_item_new();
	gtk_menu_append(GTK_MENU(context_menu),menu_item);
	menu_item = gtk_menu_item_new_with_label("Ignore");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), ignore_menu);
	gtk_menu_append(GTK_MENU(context_menu),menu_item);
	if(globals_has_ops && id !=-1)
	{
		menu_item = gtk_menu_item_new_with_label("Actions");
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), actions_menu);
		gtk_menu_append(GTK_MENU(context_menu),menu_item);
	}
#ifdef LOG
	if(id>0)
	{
		if(client_get_logged(id))
		{
			menu_item = gtk_menu_item_new_with_label("Stop Log");
		}
		else
		{
			menu_item = gtk_menu_item_new_with_label("Log");
		}
		g_signal_connect(G_OBJECT(menu_item),"activate",G_CALLBACK(client_toggle_log),(void*)id);
		gtk_menu_append(GTK_MENU(context_menu),menu_item);
	}
#endif
	
	//g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuSave),NULL);
	gtk_widget_show_all(context_menu);
	if(event)
	{
		button = event->button;
		event_time = event->time;
	}
	else
	{
		button = 3;
		event_time = gtk_get_current_event_time ();
	}
	if(button == 3)
	{
		gtk_menu_attach_to_widget(GTK_MENU(context_menu), tree_view, NULL);
		gtk_menu_popup(GTK_MENU(context_menu), NULL, NULL, NULL, NULL, button, event_time);
	}
	
	return FALSE;
}

static gboolean client_popup_menu_key(GtkWidget *widget)
{
  client_popup_menu(widget, NULL);
  return TRUE;
}

static void client_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	GtkTreeIter cur_pos;
	gtk_tree_model_get_iter(GTK_TREE_MODEL(client_list),&cur_pos, path);
	int id;
	char *nick, *name, *website, *email;
	gtk_tree_model_get(GTK_TREE_MODEL(client_list),&cur_pos,0,&nick,1,&id,3,&name,4,&website,5,&email,-1);
	if(id!=-1)
		client_info_show(nick, name, website, email);
	else
		client_info_show(nick, globals_name, globals_website, globals_email);
}

static void client_init()
{
	if(!client_tree_view)
	{
		client_tree_view = gtk_tree_view_new();
		/* Create a client list that stores their Nick name, ID, Real Name, Website, Email*/
		client_list = gtk_list_store_new(6,G_TYPE_STRING,G_TYPE_INT,G_TYPE_INT,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);
		GtkCellRenderer *client_view_rend = gtk_cell_renderer_text_new();
		/* Set the 1st column to be viewable in the treeview */
		gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(client_tree_view),-1,"Clients",client_view_rend,"text",0,NULL);
		gtk_tree_view_set_model(GTK_TREE_VIEW(client_tree_view),GTK_TREE_MODEL(client_list));
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(client_list), 0, GTK_SORT_ASCENDING);
		
		g_signal_connect(G_OBJECT(client_tree_view), "row-activated", G_CALLBACK(client_activated), NULL);
		g_signal_connect(G_OBJECT(client_tree_view), "button-press-event", G_CALLBACK(client_popup_menu), NULL);
		g_signal_connect(G_OBJECT(client_tree_view), "popup-menu", G_CALLBACK(client_popup_menu_key), NULL);
	}
}


GtkWidget *client_get_widget()
{
	client_init();
	GtkWidget *client_scrolled = gtk_scrolled_window_new(NULL,NULL);
	gtk_container_add(GTK_CONTAINER(client_scrolled),client_tree_view);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(client_scrolled),GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(client_scrolled),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(client_scrolled,105,0);
	return client_scrolled;
}

void client_add(char *nick, int id)
{
	GtkTreeIter cur_pos;
	gtk_list_store_insert(GTK_LIST_STORE(client_list),&cur_pos,0);
	gtk_list_store_set(client_list,&cur_pos,0,nick,1,id,2,0,3,"Not Set",4,"Not Set",5,"Not Set",-1);
}

void client_change(char *nname, int32_t id)
{
	GtkTreeIter cur_pos;
	int32_t nid = 0;
	char found = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(client_list),&cur_pos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(client_list),&cur_pos,1,&nid,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}
	while(gtk_tree_model_iter_next(GTK_TREE_MODEL(client_list),&cur_pos));

	if(found)
	{
		gtk_list_store_set(client_list,&cur_pos,0,nname,-1);
		if(id == -1)
		{
			char msg[128] = { 0 };
			sprintf(msg,"*** You are now known as %s\n",nname);
			chat_append(msg);
		}
	}
	else
		client_add(nname,id);
}

void client_set_name(char *nname, int32_t id)
{
	GtkTreeIter cur_pos;
	int32_t nid = 0;
	char found = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(client_list),&cur_pos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(client_list),&cur_pos,1,&nid,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}
	while(gtk_tree_model_iter_next(GTK_TREE_MODEL(client_list),&cur_pos));

	if(found)
	{
		// Set column 3 (2 since it starts at 0) to the clients name
		gtk_list_store_set(client_list,&cur_pos,3,nname,-1);
	}
	else
		fprintf(stderr, "Couldn't set the real name for client with id of %d\n",id);
}

void client_set_website(char *website, int32_t id)
{
	GtkTreeIter cur_pos;
	int32_t nid = 0;
	char found = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(client_list),&cur_pos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(client_list),&cur_pos,1,&nid,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}
	while(gtk_tree_model_iter_next(GTK_TREE_MODEL(client_list),&cur_pos));

	if(found)
	{
		// Set column 3 (2 since it starts at 0) to the clients name
		gtk_list_store_set(client_list,&cur_pos,4,website,-1);
	}
	else
		fprintf(stderr, "Couldn't set the website for client with id of %d\n",id);
}

void client_set_email(char *email, int32_t id)
{
	GtkTreeIter cur_pos;
	int32_t nid = 0;
	char found = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(client_list),&cur_pos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(client_list),&cur_pos,1,&nid,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}
	while(gtk_tree_model_iter_next(GTK_TREE_MODEL(client_list),&cur_pos));

	if(found)
	{
		// Set column 3 (2 since it starts at 0) to the clients name
		gtk_list_store_set(client_list,&cur_pos,5,email,-1);
	}
	else
		fprintf(stderr, "Couldn't set the email for client with id of %d\n",id);
}

char *client_get_name(int32_t id)
{
	GtkTreeIter cur_pos;
	int32_t nid = 0;
	char found = 0;
	char *nick;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(client_list),&cur_pos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(client_list),&cur_pos,0,&nick,1,&nid,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(client_list),&cur_pos));
	if(found)
		return nick;
	else
		return 0;	
}

int client_get_status(int32_t id)
{
	GtkTreeIter cur_pos;
	int32_t nid = 0;
	char found = 0;
	int32_t status;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(client_list),&cur_pos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(client_list),&cur_pos,2,&status,1,&nid,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(client_list),&cur_pos));
	if(found)
		return status;
	else
		return -1;	
}

int client_set_status(int32_t id, int status)
{
	/*
		TODO: Client list highlighting and stuff!
		Make it obvious who has voice rather than getting
		<charlie> Hey Tom, what's up, I heard you've got some awesome news
		...
		<charlie> Come on tom... I thought we were friends :(
		...
		<charlie> Ok get screwed I don't need you any way, and I banged your girlfriend! HA HOW DO YOU LIKE ME NOW!
	
		sorta situations
	*/
	GtkTreeIter cur_pos;
	int32_t nid = 0;
	char found = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(client_list),&cur_pos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(client_list),&cur_pos,1,&nid,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(client_list),&cur_pos));
	if(found)
	{
		// Set column 3 (2 since it starts at 0) to the clients name
		gtk_list_store_set(client_list,&cur_pos,2,status,-1);
	}
	else
		return -1;	
	return 0;
}

#ifdef LOG

void client_set_logged(int32_t id)
{
	GtkTreeIter cur_pos;
	int32_t nid = 0;
	char found = 0;
	int stats = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(client_list),&cur_pos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(client_list),&cur_pos,1,&nid,2,&stats,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(client_list),&cur_pos));
	if(found)
	{
		if((stats&CLIENT_LOG)==CLIENT_LOG)
		{
			stats^=CLIENT_LOG;
		}
		else
		{
			stats = stats|CLIENT_LOG;
		}
		gtk_list_store_set(client_list,&cur_pos,2,stats,-1);
	}
}

char client_get_logged(int32_t id)
{
	GtkTreeIter cur_pos;
	int32_t nid = 0;
	char found = 0;
	int stats = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(client_list),&cur_pos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(client_list),&cur_pos,1,&nid,2,&stats,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(client_list),&cur_pos));
	if(found)
	{
		return ((stats&CLIENT_LOG)==CLIENT_LOG);
	}
	else
		return 0;	
}

#endif

void client_remove(int32_t id)
{
	GtkTreeIter cur_pos;
	int32_t nid = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(client_list),&cur_pos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(client_list),&cur_pos,1,&nid,-1);
		if(nid == id)
		{
			gtk_list_store_remove(client_list,&cur_pos);
			return;
		}
	}
	while(gtk_tree_model_iter_next(GTK_TREE_MODEL(client_list),&cur_pos));
}

void client_clear_list()
{
	gtk_list_store_clear(client_list);
	client_add(globals_nick, -1);
}
