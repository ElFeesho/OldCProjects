#include <gtk/gtk.h>
#include <stdio.h>
#include "errors.h"
#include "database.h"
#include "browser.h"
#include "misc.h"
#include "levels.h"
#include "groups.h"
#include "gtkpmbutton.h"

struct groups_node *groups_list = NULL;
struct groups_node *groups_selected = NULL;

/* Callbacks */
static void groups_button_click(GtkWidget *button, gpointer selected);
/* Callbacks */

static int groups_count = 0;

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

void groups_create_table()
{
	if(groups_list == NULL)
	{
		error_dialog("Failed to load group list!\n");
	}	
	
	if(browser_table == NULL)
		browser_table = gtk_table_new(11,7,1);
	else
		gtk_table_resize(GTK_TABLE(browser_table),11,7);
	
	
	GtkWidget *group[3];
	struct groups_node *temp = groups_list;
	int i = 0;
	while(temp)
	{
		group[i] = gtk_pmbutton_new(temp->name); 
		g_signal_connect(G_OBJECT(group[i++]),"clicked",G_CALLBACK(groups_button_click),temp);
		temp = temp->next;
	}
	
	gtk_table_attach(GTK_TABLE(browser_table),gtk_label_new("Select an appropriate academic group"),1,6,1,2,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),group[0],1,3,4,6,GTK_EXPAND|GTK_FILL,GTK_FILL|GTK_EXPAND,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),group[1],4,6,4,6,GTK_FILL,GTK_FILL|GTK_EXPAND,0,0);
	if(groups_count==3)
	{
		gtk_table_attach(GTK_TABLE(browser_table),group[2],2,5,7,9,GTK_FILL,GTK_FILL|GTK_EXPAND,0,0);
	}
	
	gtk_widget_show_all(browser_table);
}

void groups_button_click(GtkWidget *button, gpointer selected)
{
	groups_selected = selected;
	gtk_container_forall(GTK_CONTAINER(browser_table),(GtkCallback)gtk_widget_destroy,NULL);
	levels_create_table();
	browser_add_history("Groups",groups_create_table);
}

