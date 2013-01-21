#include <gtk/gtk.h>
#include "gtkpmbutton.h"
#include "errors.h"
#include "database.h"
#include "browser.h"
#include "misc.h"
#include "levels.h"
#include "scat.h"
#include "globals.h"
#include "topics.h"
#include "stopic.h"
#include "board.h"

/* Callback Protoscat */
static void scat_button_click(GtkWidget *button, gpointer selected);
/* End of protoscat */

struct scat_node *scat_list = NULL;
struct scat_node *scat_selected = NULL;

static int scat_count = 0;

int scat_load_list()
{
	sqlite3_stmt *scat = database_query(g_strdup_printf("SELECT * FROM sub_cats INNER JOIN STopic_Subs ON sub_cats.SubCat_ID = STopic_Subs.subcat_id WHERE STopic_Subs.stopic_id = %d and STopic_Subs.topic_id = %d",stopic_selected->id,topics_selected->id));
	int count = 0;
	while(sqlite3_step(scat)==SQLITE_ROW)
	{
		if(scat_list == NULL)
		{
			scat_list = malloc(sizeof(struct scat_node));
			scat_list->name = g_strdup((char*)sqlite3_column_text(scat,1));
			
			scat_list->id = sqlite3_column_int(scat,0);
			scat_list->next = NULL;
		}
		else
		{
			struct scat_node *temp = malloc(sizeof(struct scat_node));
			temp->id = sqlite3_column_int(scat,0);
			temp->name = g_strdup((char*)sqlite3_column_text(scat,1));
			temp->next = scat_list;
			scat_list = temp;
		}
		count++;
	}
	scat_count = count;
	return count;	
}

void scat_create_table()
{
	if(!scat_list)
	{
		scat_load_list();
		if(!scat_count)
		{
			error_dialog("Couldn't find Sub-categories!");
			return;
		}
	}	
	if(browser_table == NULL)
		browser_table = gtk_table_new(8,10,1);
	else
		gtk_table_resize(GTK_TABLE(browser_table),10+scat_count,7);
	int i = 0;
	GtkWidget *label = gtk_label_new("Choose a subcategory");
	gtk_table_attach(GTK_TABLE(browser_table),label,0,7,1,2,GTK_FILL,GTK_FILL,0,0);
	
	GtkWidget *scat[scat_count];
	struct scat_node *temp = scat_list;
	for(i=0;i<scat_count && temp!=NULL;i++,temp=temp->next)
	{
		scat[i] = gtk_pmbutton_new(temp->name); 
		gtk_widget_set_size_request(scat[i],gdk_screen_get_width(gdk_screen_get_default())/5,0);
		if(i<4)
			gtk_table_attach(GTK_TABLE(browser_table),scat[i],1,3,3+i*2,(2+i*2)+3,GTK_FILL,GTK_FILL,0,0);
		else
			gtk_table_attach(GTK_TABLE(browser_table),scat[i],4,6,3+(i-4)*2,(2+(i-4)*2)+3,GTK_FILL,GTK_FILL,0,0);
		g_signal_connect(G_OBJECT(scat[i]),"clicked",G_CALLBACK(scat_button_click),temp);
	}
	GtkWidget *all_button = gtk_pmbutton_new("All");
	g_signal_connect(G_OBJECT(all_button),"clicked",G_CALLBACK(scat_button_click),NULL);
	gtk_table_attach(GTK_TABLE(browser_table),all_button,2,5,8,10,GTK_FILL,GTK_FILL,0,0);
	gtk_widget_show_all(browser_table);
	return;
}

void scat_button_click(GtkWidget *button, gpointer selected)
{
	scat_selected = selected;
	gtk_container_forall(GTK_CONTAINER(browser_table),(GtkCallback)gtk_widget_destroy,NULL);
	board_create_table(NULL);
	browser_add_history("Sub Category",scat_create_table);
}
