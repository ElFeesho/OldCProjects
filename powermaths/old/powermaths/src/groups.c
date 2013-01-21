#include <gtk/gtk.h>
#include <stdio.h>
#include "errors.h"
#include "database.h"
#include "browser.h"
#include "misc.h"
#include "levels.h"
#include "groups.h"

struct groups_node *groups_list = NULL;
struct groups_node *groups_selected = NULL;

/* Callbacks */
static void groups_button_hover(GtkWidget *button, gpointer text);
static void groups_button_leave(GtkWidget *button, gpointer text);
static void groups_button_click(GtkWidget *button, gpointer selected);
/* Callbacks */

static int groups_count = 0;
static GtkTextBuffer *details_buf = NULL;

static GtkWidget *groups_viewport = NULL;

int groups_load_list()
{
	sqlite3_stmt *groups = database_query("SELECT * FROM groups ORDER BY group_id DESC LIMIT 3");
	int count = 0;
	while(sqlite3_step(groups)==SQLITE_ROW)
	{
		if(groups_list == NULL)
		{
			groups_list = malloc(sizeof(struct groups_node));
			groups_list->caption = malloc(500);
			groups_list->name = malloc(16);
			memset(groups_list->caption,'\0',500);
			memset(groups_list->name,'\0',16);
			
			groups_list->id = sqlite3_column_int(groups,0);
			strcpy(groups_list->name, (char*)sqlite3_column_text(groups,1));
			strcpy(groups_list->caption, (char*)sqlite3_column_text(groups,2));
			groups_list->next = NULL;
		}
		else
		{
			struct groups_node *temp = malloc(sizeof(struct groups_node));
			temp->caption = malloc(500);
			temp->name = malloc(16);
			memset(temp->name,'\0',16);
			memset(temp->caption,'\0',500);
			temp->id = sqlite3_column_int(groups,0);
			strcpy(temp->name, (char*)sqlite3_column_text(groups,1));
			strcpy(temp->caption, (char*)sqlite3_column_text(groups,2));
			temp->next = groups_list;
			groups_list = temp;
		}
		count++;
	}
	groups_count = count;
	return count;	
}

GtkWidget *groups_create_viewport()
{
	if(groups_list == NULL)
	{
		error_dialog("Failed to load group list!\n");
	}	
	groups_viewport = gtk_viewport_new(NULL,NULL);
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(groups_viewport),GTK_SHADOW_NONE);
	GtkWidget *details = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(details),GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(details),0);
	
	details_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(details));
	
	GtkWidget *text_scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(text_scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(text_scroll),GTK_SHADOW_IN);
	gtk_container_add(GTK_CONTAINER(text_scroll),details);
	
	GtkWidget *table = gtk_table_new(11,8,1);
	
	
	GtkWidget *group[3];
	struct groups_node *temp = groups_list;
	int i = 0;
	while(temp)
	{
		group[i] = gtk_button_new_with_label(temp->name); 
		g_signal_connect(G_OBJECT(group[i]),"enter",G_CALLBACK(groups_button_hover),temp->caption);
		g_signal_connect(G_OBJECT(group[i]),"leave",G_CALLBACK(groups_button_leave),NULL);
		g_signal_connect(G_OBJECT(group[i++]),"clicked",G_CALLBACK(groups_button_click),temp);
		temp = temp->next;
	}
	
	gtk_table_attach(GTK_TABLE(table),misc_create_banner("Select a Category",50),0,8,0,1,GTK_FILL,GTK_FILL,0,0);
	
	gtk_table_attach(GTK_TABLE(table),group[0],1,2,2,4,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),group[1],1,2,5,7,GTK_FILL,GTK_FILL,0,0);
	if(groups_count==3)
	{
		gtk_table_attach(GTK_TABLE(table),group[2],1,2,8,10,GTK_FILL,GTK_FILL,0,0);
		gtk_table_attach(GTK_TABLE(table),text_scroll,3,7,2,10,GTK_FILL,GTK_FILL|GTK_EXPAND,0,0);
	}
	else
	{
		gtk_table_attach(GTK_TABLE(table),text_scroll,3,7,2,10,GTK_FILL,GTK_FILL|GTK_EXPAND,0,0);
	}
	
	gtk_container_add(GTK_CONTAINER(groups_viewport),table);
	
	g_signal_connect(G_OBJECT(table),"expose-event",G_CALLBACK(browser_viewport_expose),NULL);	
		
	return groups_viewport;
}

void groups_button_click(GtkWidget *button, gpointer selected)
{
	groups_selected = selected;
	browser_change_viewport(groups_viewport,levels_create_viewport());
}

void groups_button_hover(GtkWidget *button, gpointer text)
{
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(details_buf, &start, &end);
	gtk_text_buffer_delete(details_buf,&start, &end);
	gtk_text_buffer_insert(details_buf,&start,text,-1);
}

void groups_button_leave(GtkWidget *button, gpointer text)
{
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(details_buf, &start, &end);
	gtk_text_buffer_delete(details_buf,&start, &end);
}
