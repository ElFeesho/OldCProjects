/*
 * List of all the questions from the filtering!
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "draw.h"
#include "errors.h"
#include "misc.h"
#include "question.h"
#include "whiteb.h"
#include "browser.h"

/*	Callbacks */
static gboolean board_window_delete(GtkWidget *window, gpointer user_data);
static gboolean board_thumbnail_expose(GtkWidget *thumb, GdkEventExpose *ev, gpointer user_data);
static gboolean board_thumbail_press(GtkWidget *thumb, GdkEventButton *event, gpointer user_data);
/* End of Callbacks */

static GtkWidget *board_create_thumbnail(void *qdata);

void board_show(char *name, char *table_name)
{
	/* Layout Widgets */
	GtkWidget *window;
	GtkWidget *table_adapter;
	GtkWidget *table;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"Powermaths");
	gtk_window_set_default_size(GTK_WINDOW(window),800,600);
	table_adapter = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(table_adapter),GTK_SHADOW_IN);
	
	if(question_load_questions(table_name)<1)
	{
		error_dialog("No questions were loaded!");
		board_window_delete(window,NULL);
	}
	
	table = gtk_table_new(9,(question_get_total()/10)+1,0);
	gtk_table_set_row_spacings(GTK_TABLE(table),20);
	gtk_table_set_col_spacings(GTK_TABLE(table),20);

	/* Pack widgets */
	gtk_container_add(GTK_CONTAINER(window),table_adapter);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(table_adapter),table);
	
	gtk_table_attach(GTK_TABLE(table),misc_create_banner(g_strdup_printf("Select a Question - %s",name),25000),0,9,0,1,GTK_EXPAND|GTK_FILL,0,0,0);
	
	int i = 0;
	struct QuestionNode *iter = qlist;
	while(iter)
	{
		gtk_table_attach(GTK_TABLE(table),board_create_thumbnail(iter->qstn),i-((i/5)*5),i-((i/5)*5)+1,(i/5)+1,(i/5)+2,0,0,0,0);
		i++;
		iter = iter->next;
	}
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(board_window_delete),NULL);

	gtk_widget_show_all(window);
}

gboolean board_window_delete(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	browser_show();
	return 1;
}

gboolean board_thumbnail_expose(GtkWidget *thumb, GdkEventExpose *ev, gpointer user_data)
{
	draw_thumbnail_question(thumb,user_data);
	return 1;
}

GtkWidget *board_create_thumbnail(void *qdata)
{
	GtkWidget *thumb = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(thumb),160,120);
	gtk_widget_modify_bg(thumb,GTK_STATE_NORMAL,misc_create_color(65535,65535,65535));
	gtk_widget_add_events(thumb,gtk_widget_get_events(thumb)|GDK_BUTTON_PRESS_MASK);
	g_signal_connect(G_OBJECT(thumb),"expose-event",G_CALLBACK(board_thumbnail_expose),qdata);
	g_signal_connect(G_OBJECT(thumb),"button-press-event",G_CALLBACK(board_thumbail_press),qdata);
	return thumb;
}

gboolean board_thumbail_press(GtkWidget *thumb, GdkEventButton *event, gpointer user_data)
{
	whiteb_show(user_data);
	return 1;
}
