#include <gtk/gtk.h>
#include <stdio.h>
#include <stdint.h>
#include "globals.h"
#include "chat.h"


static GtkWidget *client_tree_view = 0;
static GtkListStore *client_list = 0;

static void client_init()
{
	if(!client_tree_view)
	{
		client_tree_view = gtk_tree_view_new();
		client_list = gtk_list_store_new(2,G_TYPE_STRING,G_TYPE_INT);
		GtkCellRenderer *client_view_rend = gtk_cell_renderer_text_new();
		gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(client_tree_view),-1,"Clients",client_view_rend,"text",0,NULL);
		gtk_tree_view_set_model(GTK_TREE_VIEW(client_tree_view),GTK_TREE_MODEL(client_list));
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(client_list), 0, GTK_SORT_ASCENDING);
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
	gtk_list_store_set(client_list,&cur_pos,0,nick,1,id,-1);
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
