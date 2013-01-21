#include <gtk/gtk.h>
#include <stdio.h>
#include "browser.h"
#include "groups.h"
#include "levels.h"
#include "gtkpmbutton.h"
#include "misc.h"
#include "board.h"

static GtkListStore *groups_cmb = NULL;
static GtkListStore *levels_cmb = NULL;
static GtkWidget *levels_combo = NULL;
static GtkWidget *groups_combo  = NULL;

static int level_id = -1;
static int group_id = -1;

static void search_groups_changed(GtkWidget *groups_combo, gpointer user_data)
{
	GtkTreeIter cur_pos;
	int *id;
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(groups_combo),&cur_pos);
	gtk_tree_model_get(GTK_TREE_MODEL(groups_cmb),&cur_pos,0,&id,-1);

	group_id = *id;
	gtk_list_store_clear(levels_cmb);
	struct levels_node *level_search_list = levels_load_list_from_group_id(*id);

	if(level_search_list!=NULL)
	{
		struct levels_node *temp = level_search_list;
		GtkTreeIter list_pos;
		gtk_list_store_append(levels_cmb,&list_pos);
		int na_id = -1;
		gtk_list_store_set(levels_cmb,&list_pos,0,&na_id,1,"N/A",-1);
		while(temp!=NULL)
		{
			gtk_list_store_append(levels_cmb,&list_pos);
			gtk_list_store_set(levels_cmb,&list_pos,0,&temp->id,1,temp->name,-1);
			temp = temp->next;
		}
	}
	level_id =-1;
	gtk_combo_box_set_active(GTK_COMBO_BOX(levels_combo),0);
}

static void search_levels_changed(GtkWidget *levels_combo, gpointer user_data)
{
	GtkTreeIter cur_pos;
	int *id;
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(levels_combo),&cur_pos);
	gtk_tree_model_get(GTK_TREE_MODEL(levels_cmb),&cur_pos,0,&id,-1);

	level_id = *id;
}

static void search_clicked(GtkWidget *button, GtkWidget *text)
{
   int count = 0;
   char **split_keywords = misc_tokenize_string(gtk_entry_get_text(GTK_ENTRY(text)),' ',&count);
   
   char *query = g_malloc0(1024);
   sprintf(query,"SELECT questions.caption, questions.filename, Groups.group_name, Grades.grade_name, types.type_name, topics.topic_name, sub_topics.stopic_name FROM questions INNER JOIN Groups ON Groups.Group_ID = questions.Groups INNER JOIN Grades ON Grades.Grade_ID = questions.level INNER JOIN types ON types.type_id = questions.Type INNER JOIN topics ON topics.topic_id = questions.topic INNER JOIN sub_topics ON sub_topics.Subtopic_ID = questions.stopic WHERE Questions.Groups = %d",group_id);
	if(level_id!=-1)
		sprintf(query,"%s AND Questions.level = %d",query,level_id);
   int i = 0;
   for(i = 0;i<count;i++)
   {
      sprintf(query,"%s AND Questions.keywords LIKE \"%%%s%%\"",query,split_keywords[i]);
   }
   sprintf(query,"%s ORDER BY Questions.quest_id DESC",query);
   board_create_table(query);
   
}

void search_create_table()
{
	groups_cmb = gtk_list_store_new(2,G_TYPE_INT,G_TYPE_STRING);
	levels_cmb = gtk_list_store_new(2,G_TYPE_INT,G_TYPE_STRING);
	groups_combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(groups_cmb));
	levels_combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(levels_cmb));

	if(groups_list!=NULL)
	{
		struct groups_node *temp = groups_list;
		GtkTreeIter list_pos;
		while(temp!=NULL)
		{
			gtk_list_store_append(groups_cmb,&list_pos);
			gtk_list_store_set(groups_cmb,&list_pos,0,&temp->id,1,temp->name,-1);
			temp=temp->next;
		}
	}
	
	struct levels_node *level_search_list = levels_load_list_from_group_id(groups_list->id);
	if(level_search_list!=NULL)
	{
		struct levels_node *temp = level_search_list;
		GtkTreeIter list_pos;
		int na_id = -1;
		gtk_list_store_append(levels_cmb,&list_pos);
		gtk_list_store_set(levels_cmb,&list_pos,0,&na_id,1,"N/A",-1);	
		while(temp!=NULL)
		{
			gtk_list_store_append(levels_cmb,&list_pos);
			gtk_list_store_set(levels_cmb,&list_pos,0,&temp->id,1,temp->name,-1);
			temp = temp->next;
		}
	}
	level_id = -1;
	
	GtkCellRenderer *group_rend = gtk_cell_renderer_text_new();
	GtkCellRenderer *level_rend = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(groups_combo),group_rend,0);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(groups_combo),group_rend,"text",1,NULL);
	
	gtk_combo_box_set_active(GTK_COMBO_BOX(groups_combo),0);
	group_id = groups_list->id;
	
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(levels_combo),level_rend,0);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(levels_combo),level_rend,"text",1,NULL);
	
	gtk_combo_box_set_active(GTK_COMBO_BOX(levels_combo),0);
	
	
	gtk_container_forall(GTK_CONTAINER(browser_table),(GtkCallback)gtk_widget_destroy,NULL);
	
	if(browser_table == NULL)
		browser_table = gtk_table_new(13,8,0);
	else
		gtk_table_resize(GTK_TABLE(browser_table),11,8);
	GtkWidget *search = gtk_pmbutton_new("Search");
	GtkWidget *search_text = gtk_entry_new_with_max_length(20);
	gtk_table_attach(GTK_TABLE(browser_table),gtk_label_new("Select a group"),1,3,1,2,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),groups_combo,2,4,2,3,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),gtk_label_new("Select a level"),1,3,4,5,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),levels_combo,2,4,5,6,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),gtk_label_new("Enter some keywords"),1,4,7,8,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),search_text,2,4,8,9,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(browser_table),search,2,5,9,11,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
	
	g_signal_connect(G_OBJECT(groups_combo),"changed",G_CALLBACK(search_groups_changed),NULL);
	g_signal_connect(G_OBJECT(levels_combo),"changed",G_CALLBACK(search_levels_changed),NULL);
	g_signal_connect(G_OBJECT(search),"clicked",G_CALLBACK(search_clicked),search_text);
	gtk_widget_show_all(browser_table);
}
