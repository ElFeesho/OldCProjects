#include <gtk/gtk.h>
#include <stdio.h>
#include "database.h"
#include "browser.h"
#include "misc.h"
#include "globals.h"
#include "errors.h"
#include "topics.h"
#include "types.h"
#include "gtkpmbutton.h"

/* Callback Declarations */
static void topics_button_click(GtkWidget *button, gpointer selected);
/* End of callbacks */

struct topics_node *topics_selected;
static struct topics_node *topics_list = NULL;

static int topics_count = 0;

int topics_load_list()
{
	sqlite3_stmt *topics = database_query(g_strdup_printf("SELECT Topics.topic_id, Topics.topic_name FROM topics INNER JOIN Type_Topics ON topics.Topic_ID = Type_Topics.Topic_ID WHERE Type_Topics.Type_ID = %d",types_selected->id));
	int count = 0;
	while(sqlite3_step(topics)==SQLITE_ROW)
	{
		if(topics_list == NULL)
		{
			topics_list = malloc(sizeof(struct topics_node));
			topics_list->name = malloc(64);
			memset(topics_list->name,'\0',64);
			
			topics_list->id = sqlite3_column_int(topics,0);
			strcpy(topics_list->name, (char*)sqlite3_column_text(topics,1));
			topics_list->next = NULL;
		}
		else
		{
			struct topics_node *temp = malloc(sizeof(struct topics_node));
			temp->name = malloc(64);
			memset(temp->name,'\0',64);
			temp->id = sqlite3_column_int(topics,0);
			strcpy(temp->name, (char*)sqlite3_column_text(topics,1));
			temp->next = topics_list;
			topics_list = temp;
		}
		count++;
	}
	topics_count = count;
	return count;	
}


void topics_create_table()
{
	if(!topics_load_list())
	{
		error_dialog("Failed loading topics!");
		exit(1);
	}
	
	if(!browser_table)
		browser_table = gtk_table_new(topics_count+4,5,1);
	else
	{
		gtk_table_resize(GTK_TABLE(browser_table),topics_count+4,6);
	}
	
	gtk_table_attach_defaults(GTK_TABLE(browser_table),gtk_label_new("Choose a topic for the questions."),0,5,1,2);
	
	GtkWidget *topic[10];
	int i = 0;
	struct topics_node *temp = topics_list;
	for(i=0;i<topics_count && temp!=NULL;i++,temp=temp->next)
	{
		topic[i] = gtk_pmbutton_new(temp->name);
		gtk_widget_set_size_request(topic[i],gdk_screen_get_width(gdk_screen_get_default())/5,0);
		if(i<4)
			gtk_table_attach(GTK_TABLE(browser_table),topic[i],1,2,3+i*2,(2+i*2)+3,GTK_FILL,GTK_FILL,0,0);
		else
			gtk_table_attach(GTK_TABLE(browser_table),topic[i],3,4,3+(i-4)*2,(2+(i-4)*2)+3,GTK_FILL,GTK_FILL,0,0);
		g_signal_connect(G_OBJECT(topic[i]),"clicked",G_CALLBACK(topics_button_click),temp);
	}
	gtk_widget_show_all(browser_table);
	return;
}

void topics_button_click(GtkWidget *button, gpointer selected)
{
	topics_selected = selected;
	gtk_container_forall(GTK_CONTAINER(browser_table),(GtkCallback)gtk_widget_destroy,NULL);
	stopic_create_table();
	browser_add_history("Topics",topics_create_table);
}
