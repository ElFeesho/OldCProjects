#include <gtk/gtk.h>
#include "gtkpmbutton.h"
#include "errors.h"
#include "database.h"
#include "browser.h"
#include "misc.h"
#include "levels.h"
#include "stopic.h"
#include "scat.h"
#include "globals.h"
#include "board.h"

/* Callback Protostopic */
static void stopic_button_click(GtkWidget *button, gpointer selected);
/* End of protostopic */

struct stopic_node *stopic_list = NULL;
struct stopic_node *stopic_selected = NULL;

static int stopic_count = 0;

int stopic_load_list()
{
	sqlite3_stmt *stopic = database_query(g_strdup_printf("SELECT * FROM sub_topics INNER JOIN Topic_Subs ON sub_topics.Subtopic_ID = Topic_Subs.stopic_id WHERE Topic_Subs.topic_id = %d",topics_selected->id));
	int count = 0;
	while(sqlite3_step(stopic)==SQLITE_ROW)
	{
		if(stopic_list == NULL)
		{
			stopic_list = malloc(sizeof(struct stopic_node));
			stopic_list->name = g_strdup((char*)sqlite3_column_text(stopic,1));
			
			stopic_list->id = sqlite3_column_int(stopic,0);
			stopic_list->next = NULL;
		}
		else
		{
			struct stopic_node *temp = malloc(sizeof(struct stopic_node));
			temp->id = sqlite3_column_int(stopic,0);
			temp->name = g_strdup((char*)sqlite3_column_text(stopic,1));
			temp->next = stopic_list;
			stopic_list = temp;
		}
		count++;
	}
	stopic_count = count;
	return count;	
}

void stopic_create_table()
{
	if(!stopic_list)
	{
		stopic_load_list();
		if(!stopic_count)
		{
			error_dialog("Couldn't find Sub-topics!");
			return;
		}
	}	
	if(browser_table == NULL)
		browser_table = gtk_table_new(8,7,1);
	else
		gtk_table_resize(GTK_TABLE(browser_table),10+stopic_count,7);
	int i = 0;
	GtkWidget *label = gtk_label_new("Choose a subtopic");
	gtk_table_attach(GTK_TABLE(browser_table),label,0,7,1,2,GTK_FILL,GTK_FILL,0,0);
	
	GtkWidget *stopic[stopic_count];
	struct stopic_node *temp = stopic_list;
	for(i=0;i<stopic_count && temp!=NULL;i++,temp=temp->next)
	{
		stopic[i] = gtk_pmbutton_new(temp->name); 
		gtk_widget_set_size_request(stopic[i],gdk_screen_get_width(gdk_screen_get_default())/5,0);
		if(i<4)
			gtk_table_attach(GTK_TABLE(browser_table),stopic[i],1,3,3+i*2,(2+i*2)+3,GTK_FILL,GTK_FILL,0,0);
		else
			gtk_table_attach(GTK_TABLE(browser_table),stopic[i],4,6,3+(i-4)*2,(2+(i-4)*2)+3,GTK_FILL,GTK_FILL,0,0);
		g_signal_connect(G_OBJECT(stopic[i]),"clicked",G_CALLBACK(stopic_button_click),temp);
	}
	
	gtk_widget_show_all(browser_table);
	return;
}

void stopic_button_click(GtkWidget *button, gpointer selected)
{
	stopic_selected = selected;
	gtk_container_forall(GTK_CONTAINER(browser_table),(GtkCallback)gtk_widget_destroy,NULL);
	printf(g_strdup_printf("SELECT * FROM stopic_subs WHERE topic_id = %d AND stopic_id = %d\n",topics_selected->id,((struct stopic_node*)selected)->id));	
	sqlite3_stmt *subcat = database_query(g_strdup_printf("SELECT * FROM stopic_subs WHERE topic_id = %d AND stopic_id = %d",topics_selected->id,((struct stopic_node*)selected)->id));
	if(sqlite3_step(subcat)==SQLITE_ROW)
		scat_create_table();
	else
		board_create_table(NULL);
	browser_add_history("Sub Topic",stopic_create_table);
}
