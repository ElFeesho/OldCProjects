#include <gtk/gtk.h>
#include "draw.h"
#include "errors.h"
#include "database.h"
#include "browser.h"
#include "misc.h"
#include "levels.h"
#include "board.h"
#include "globals.h"
#include "whiteb.h"
#include "question.h"


static GtkWidget *board_drawing_area_new(struct question *data);

void board_create_table(char *sql)
{
	 
	int questions;
	if(sql!=NULL)
	    questions = question_load_questions(sql);
	else
	{
		char *query = g_malloc0(1024);
		sprintf(query,"SELECT questions.caption, questions.filename, Groups.group_name, Grades.grade_name, types.type_name, topics.topic_name, sub_topics.stopic_name FROM questions INNER JOIN Groups ON Groups.Group_ID = questions.Groups INNER JOIN Grades ON Grades.Grade_ID = questions.level INNER JOIN types ON types.type_id = questions.Type INNER JOIN topics ON topics.topic_id = questions.topic INNER JOIN sub_topics ON sub_topics.Subtopic_ID = questions.stopic INNER JOIN sub_cats ON sub_cats.Subcat_ID = questions.scat WHERE Questions.Groups = %d AND Questions.Topic = %d AND Questions.STopic = %d",groups_selected->id, topics_selected->id, stopic_selected->id);
		if(levels_selected!=NULL)
			sprintf(query,"%s AND Questions.level = %d",query,levels_selected->id);
		if(scat_selected!=NULL)
			sprintf(query,"%s AND Questions.Scat = %d",query,scat_selected->id);
		sprintf(query,"%s ORDER BY Questions.quest_id DESC",query);
		printf("Query: %s\n",query);
		questions = question_load_questions(query);
	}
	
	if(!questions)
	{
		error_dialog("No questions were found!");
		 return;	
	}
	
	gtk_container_forall(GTK_CONTAINER(browser_table),(GtkCallback)gtk_widget_destroy,NULL);
	if(browser_table == NULL)
		browser_table = gtk_table_new(7,20,1);
	else
	{
		//gtk_table_resize(GTK_TABLE(browser_table),questions/3*5+1,16);
		gtk_table_resize(GTK_TABLE(browser_table),questions/3*3+1,10);
	}
	
	int q_num = 0;
	struct quest_node *temp = quest_list;
	while(temp != NULL)
	{
		temp->quest->number = ++q_num;
		GtkWidget *quest_canvas = board_drawing_area_new(temp->quest);
		
		gtk_table_attach(GTK_TABLE(browser_table),quest_canvas,1+((q_num-(((q_num-1)/3)*3)-1)*3), 3+((q_num-(((q_num-1)/3)*3)-1)*3), 1+(((q_num-1)/3)*3),3+(((q_num-1)/3)*3),GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,0,0);
//		gtk_table_attach(GTK_TABLE(browser_table),quest_canvas,1+((q_num-(((q_num-1)/5)*5)-1)*3), 3+((q_num-(((q_num-1)/5)*5)-1)*3), 1+(((q_num-1)/5)*3),3+(((q_num-1)/5)*3),GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,0,0);
		temp=temp->next;
	}
	gtk_widget_show_all(browser_table);
	return;
}

GtkWidget *board_drawing_area_new(struct question *data)
{
	GtkWidget *drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(drawing_area,gdk_screen_get_width(gdk_screen_get_default())/8,gdk_screen_get_height(gdk_screen_get_default())/8);
	gtk_widget_add_events(drawing_area,GDK_BUTTON_PRESS_MASK);
	g_signal_connect(G_OBJECT(drawing_area),"expose-event",G_CALLBACK(draw_thumbnail_question),data);
	g_signal_connect_swapped(G_OBJECT(drawing_area),"button-press-event",G_CALLBACK(whiteb_show),data);
	return drawing_area;
}
