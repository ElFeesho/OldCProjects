#include <gtk/gtk.h>
#include "errors.h"
#include "database.h"
#include "browser.h"
#include "misc.h"
#include "levels.h"
#include "types.h"
#include "topics.h"
#include "gtkpmbutton.h"

/* Callback Prototypes */
static void types_button_click(GtkWidget *button, gpointer selected);
/* End of prototypes */

struct types_node *types_list = NULL;
struct types_node *types_selected = NULL;

static int types_count = 0;

int types_load_list()
{
	sqlite3_stmt *types = database_query("SELECT * FROM types LIMIT 4");
	int count = 0;
	while(sqlite3_step(types)==SQLITE_ROW)
	{
		if(types_list == NULL)
		{
			types_list = malloc(sizeof(struct types_node));
			types_list->name = malloc(32);
			
			memset(types_list->name,'\0',32);
			
			types_list->id = sqlite3_column_int(types,0);
			strcpy(types_list->name, (char*)sqlite3_column_text(types,1));
			types_list->next = NULL;
		}
		else
		{
			struct types_node *temp = malloc(sizeof(struct types_node));
			temp->name = malloc(32);
			memset(temp->name,'\0',32);
			temp->id = sqlite3_column_int(types,0);
			strcpy(temp->name, (char*)sqlite3_column_text(types,1));
			temp->next = types_list;
			types_list = temp;
		}
		count++;
	}
	types_count = count;
	return count;	
}

void types_create_table()
{
	if(!types_list)
	{
		types_load_list();
		if(!types_count)
		{
			error_dialog("Failed to load question types!");
			exit(0);
		}
	}	
	
	if(browser_table == NULL)
		browser_table = gtk_table_new(8,7,1);
	else
		gtk_table_resize(GTK_TABLE(browser_table),8,7);
	
	
	GtkWidget *types[4];
	struct types_node *temp = types_list;
	int i = 0;
	while(temp)
	{
		types[i] = gtk_pmbutton_new(temp->name); 
		gtk_widget_set_size_request(types[i],gdk_screen_get_width(gdk_screen_get_default())/3,0);
		g_signal_connect(G_OBJECT(types[i++]),"clicked",G_CALLBACK(types_button_click),temp);
		temp = temp->next;
	}
	
	gtk_table_attach(GTK_TABLE(browser_table),gtk_label_new("Select a Question Type"),2,7,1,2,0,0,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),types[0],1,4,3,5,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),types[1],1,4,6,8,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),types[2],5,8,3,5,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),types[3],5,8,6,8,GTK_FILL,GTK_FILL,0,0);
	
	
	gtk_widget_show_all(browser_table);
	return;
}

void types_button_click(GtkWidget *button, gpointer selected)
{
	types_selected = selected;
	gtk_container_forall(GTK_CONTAINER(browser_table),(GtkCallback)gtk_widget_destroy,NULL);
	topics_create_table();
	browser_add_history("Type",types_create_table);
}
