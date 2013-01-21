#include <gtk/gtk.h>
#include <stdio.h>
#include "database.h"
#include "gtkpmbutton.h"
#include "browser.h"
#include "misc.h"
#include "globals.h"
#include "errors.h"
#include "levels.h"
#include "types.h"

/* Callback Declarations */
static void levels_button_click(GtkWidget *button, gpointer selected);
/* End of callbacks */

struct levels_node *levels_selected;
static struct levels_node *levels_list = NULL;

static int levels_count = 0;

int levels_load_list()
{
	if(levels_list!=NULL)
	{
		free(levels_list);
		levels_list = NULL;
	}

	sqlite3_stmt *levels = database_query(g_strdup_printf("SELECT * from Grades INNER JOIN Group_Grades ON Group_Grades.Grade_ID = Grades.Grade_ID WHERE Group_Grades.Group_ID = %d ORDER BY Grades.Grade_ID DESC LIMIT 10",groups_selected->id));
	int count = 0;
	while(sqlite3_step(levels)==SQLITE_ROW)
	{
		if(levels_list == NULL)
		{
			levels_list = malloc(sizeof(struct levels_node));
			levels_list->id = sqlite3_column_int(levels,0);
			levels_list->name = g_strdup((char*)sqlite3_column_text(levels,1));
			levels_list->next = NULL;
		}
		else
		{
			struct levels_node *temp = malloc(sizeof(struct levels_node));
			temp->id = sqlite3_column_int(levels,0);
			temp->name = g_strdup((char*)sqlite3_column_text(levels,1));
			temp->next = levels_list;
			levels_list = temp;
		}
		count++;
	}
	levels_count = count;
	return count;	
}

struct levels_node *levels_load_list_from_group_id(int id)
{
	struct levels_node *return_list = NULL;
	
	sqlite3_stmt *levels = database_query(g_strdup_printf("SELECT * from Grades INNER JOIN Group_Grades ON Group_Grades.Grade_ID = Grades.Grade_ID WHERE Group_Grades.Group_ID = %d ORDER BY Grades.Grade_ID DESC LIMIT 10",id));
	while(sqlite3_step(levels)==SQLITE_ROW)
	{
		if(return_list == NULL)
		{
			return_list = malloc(sizeof(struct levels_node));
			return_list->id = sqlite3_column_int(levels,0);
			return_list->name = g_strdup((char*)sqlite3_column_text(levels,1));
			return_list->next = NULL;
		}
		else
		{
			struct levels_node *temp = malloc(sizeof(struct levels_node));
			temp->id = sqlite3_column_int(levels,0);
			temp->name = g_strdup((char*)sqlite3_column_text(levels,1));
			temp->next = return_list;
			return_list = temp;
		}
	}
	return return_list;	
}

void levels_create_table()
{
	if(!levels_load_list())
	{
		error_dialog("Failed loading grades!");
		exit(1);
	}
	if(!browser_table)
		browser_table = gtk_table_new(7,6,1);
	else
		gtk_table_resize(GTK_TABLE(browser_table),12,6);

	
	gtk_table_attach_defaults(GTK_TABLE(browser_table),gtk_label_new("Which level you choose, reflects\nthe difficulty of the questions."),0,6,0,2);
	
	GtkWidget *level[10];
	int i = 0;
	struct levels_node *temp = levels_list;
	if(levels_count>0 && levels_count <=4) // Custom
		for(i=0;i<levels_count && temp!=NULL;i++,temp=temp->next)
		{
			level[i] = gtk_pmbutton_new(temp->name);
			gtk_widget_set_size_request(level[i],gdk_screen_get_width(gdk_screen_get_default())/6,0);
			if(i/2<1)
				gtk_table_attach(GTK_TABLE(browser_table),level[i],1,3,2+i*3,(2+i*3)+2,GTK_FILL,GTK_FILL,0,0);
			else if(i/2<2)
				gtk_table_attach(GTK_TABLE(browser_table),level[i],3,5,2+(i-2)*3,(2+(i-2)*3)+2,GTK_FILL,GTK_FILL,0,0);
			g_signal_connect(G_OBJECT(level[i]),"clicked",G_CALLBACK(levels_button_click),temp);
		}
	else if(levels_count>4 && levels_count <=6) // KS3
		for(i=0;i<levels_count && temp!=NULL;i++,temp=temp->next)
		{
			level[i] = gtk_pmbutton_new(temp->name);
			gtk_widget_set_size_request(level[i],gdk_screen_get_width(gdk_screen_get_default())/6,0);
			if(i/2<1)
				gtk_table_attach(GTK_TABLE(browser_table),level[i],0,2,2+i*3,(2+i*3)+2,GTK_FILL,GTK_FILL,0,0);
			else if(i/2<2)
				gtk_table_attach(GTK_TABLE(browser_table),level[i],2,4,2+(i-2)*3,(2+(i-2)*3)+2,GTK_FILL,GTK_FILL,0,0);
			else if(i/2<3)
				gtk_table_attach(GTK_TABLE(browser_table),level[i],4,6,2+(i-4)*3,(2+(i-4)*3)+2,GTK_FILL,GTK_FILL,0,0);
			g_signal_connect(G_OBJECT(level[i]),"clicked",G_CALLBACK(levels_button_click),temp);
		}
	else if(levels_count>6) // GCSE
		for(i=0;i<levels_count && temp!=NULL;i++,temp=temp->next)
		{
			level[i] = gtk_pmbutton_new(temp->name);
			gtk_widget_set_size_request(level[i],gdk_screen_get_width(gdk_screen_get_default())/6,0);
			if(i/2<1)
				gtk_table_attach(GTK_TABLE(browser_table),level[i],1,2,2+i*3,(2+i*3)+2,GTK_FILL,GTK_FILL,0,0);
			else if(i/2<2)
				gtk_table_attach(GTK_TABLE(browser_table),level[i],2,3,2+(i-2)*3,(2+(i-2)*3)+2,GTK_FILL,GTK_FILL,0,0);
			else if(i/2<3)
				gtk_table_attach(GTK_TABLE(browser_table),level[i],3,4,2+(i-4)*3,(2+(i-4)*3)+2,GTK_FILL,GTK_FILL,0,0);
			else if(i/2<4)
				gtk_table_attach(GTK_TABLE(browser_table),level[i],4,5,2+(i-6)*3,(2+(i-6)*3)+2,GTK_FILL,GTK_FILL,0,0);
			g_signal_connect(G_OBJECT(level[i]),"clicked",G_CALLBACK(levels_button_click),temp);
		}
	GtkWidget *all_button = gtk_pmbutton_new("All");
	g_signal_connect(G_OBJECT(all_button),"clicked",G_CALLBACK(levels_button_click),NULL);
	gtk_widget_set_size_request(all_button,gdk_screen_get_width(gdk_screen_get_default())/6,0);
	gtk_table_attach(GTK_TABLE(browser_table),all_button,2,4,8,10,GTK_FILL,GTK_FILL,0,0);
	gtk_widget_show_all(browser_table);
	return;
}

void levels_button_click(GtkWidget *button, gpointer selected)
{
	levels_selected = selected;
	gtk_container_forall(GTK_CONTAINER(browser_table),(GtkCallback)gtk_widget_destroy,NULL);
	types_create_table();
	browser_add_history("Levels",levels_create_table);
}
