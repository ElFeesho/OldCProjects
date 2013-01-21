#include <gtk/gtk.h>
#include <stdio.h>
#include "database.h"
#include "browser.h"
#include "misc.h"
#include "globals.h"
#include "errors.h"
#include "levels.h"

static struct levels_node *levels_list = NULL;

static int levels_count = 0;

int levels_load_list()
{
	if(levels_list!=NULL)
	{
		free(levels_list);
		levels_list = NULL;
	}
	char sql[255] = { '\0' };
	sprintf(sql,"SELECT * FROM grades WHERE group_id = %d ORDER BY grade_id DESC LIMIT 10",groups_selected->id);
	sqlite3_stmt *levels = database_query(sql);
	int count = 0;
	while(sqlite3_step(levels)==SQLITE_ROW)
	{
		if(levels_list == NULL)
		{
			levels_list = malloc(sizeof(struct levels_node));
			levels_list->grade_name = malloc(10);
			memset(levels_list->grade_name,'\0',10);
			
			levels_list->id = sqlite3_column_int(levels,0);
			strcpy(levels_list->grade_name, (char*)sqlite3_column_text(levels,2));
			levels_list->next = NULL;
		}
		else
		{
			struct levels_node *temp = malloc(sizeof(struct levels_node));
			temp->grade_name = malloc(10);
			memset(temp->grade_name,'\0',10);
			temp->id = sqlite3_column_int(levels,0);
			strcpy(temp->grade_name, (char*)sqlite3_column_text(levels,2));
			temp->next = levels_list;
			levels_list = temp;
		}
		count++;
	}
	levels_count = count;
	return count;	
}


GtkWidget *levels_create_viewport()
{
	if(!levels_load_list())
	{
		error_dialog("Failed loading grades!");
		exit(1);
	}

	GtkWidget *viewport = gtk_viewport_new(NULL,NULL);
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(viewport),GTK_SHADOW_NONE);
	
	GtkWidget *table = gtk_table_new(levels_count+5,8,1);

	gtk_table_attach(GTK_TABLE(table),misc_create_banner("Select a Level",50),0,8,0,1,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Which level you choose, reflects the difficulty of the questions."),0,8,2,3);
	
	GtkWidget *level[10];
	int i = 0;
	struct levels_node *temp = levels_list;
	for(i=0;i<levels_count && temp!=NULL;i++,temp=temp->next)
	{
		level[i] = gtk_button_new_with_label(temp->grade_name);
		gtk_table_attach(GTK_TABLE(table),level[i],1,2,4+i*2,5+i*2,GTK_FILL,GTK_FILL,0,0);
	}

	gtk_container_add(GTK_CONTAINER(viewport),table);
	g_signal_connect(G_OBJECT(table),"expose-event",G_CALLBACK(browser_viewport_expose),NULL);	
	
	return viewport;
}
