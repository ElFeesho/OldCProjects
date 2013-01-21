#include <gtk/gtk.h>
#include <string.h>
#include "client.h"

static GtkWidget *chat_history = 0;
static GtkTextBuffer *chat_history_buffer = 0;
static GtkWidget *chat_scrolled = 0;

static void chat_initialise()
{
	if(!chat_history && !chat_history_buffer)
	{
		chat_history = gtk_text_view_new();
		chat_history_buffer = gtk_text_buffer_new(NULL);
		gtk_text_view_set_buffer(GTK_TEXT_VIEW(chat_history),chat_history_buffer);
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat_history),GTK_WRAP_WORD_CHAR);
		GtkTextTag *chat_blue, *chat_purple, *chat_bold;
		chat_blue = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(chat_history_buffer),"blue","foreground","#0000ff","weight","bold",NULL);
		chat_purple = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(chat_history_buffer),"purple","foreground","#990099","weight","bold",NULL);
		chat_bold = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(chat_history_buffer),"bold","weight","bold",NULL);
		gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_history),0);

	}
}

GtkWidget *chat_get_history_widget()
{
	chat_scrolled = gtk_scrolled_window_new(NULL,NULL);
	chat_initialise();
	gtk_container_add(GTK_CONTAINER(chat_scrolled),chat_history);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(chat_scrolled),GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(chat_scrolled),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	return chat_scrolled;
}

static void chat_scroll_to_end()
{
	if(chat_scrolled)
	{
		GtkAdjustment *spos = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(chat_scrolled));
		if(spos->value == spos->upper-spos->page_size)
		{
			GtkTextIter end;
			gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
			GtkTextMark *pos = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(chat_history_buffer),NULL,&end,0);
			gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(chat_history),pos,0,1,1,1);
		}
	}
}

void chat_append(char *message)
{
	chat_initialise();
	GtkTextIter end;
	gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
	gtk_text_buffer_insert(chat_history_buffer,&end,message,-1);
	chat_scroll_to_end();
}

void chat_add_msg(int32_t id, char *msg, int32_t len)
{
	char *nick = client_get_name(id);
	if(nick)
	{
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		if(id!=-1)
			gtk_text_buffer_insert_with_tags_by_name(chat_history_buffer,&end,"<",1,"blue",NULL);
		else
			gtk_text_buffer_insert_with_tags_by_name(chat_history_buffer,&end,"<",1,"purple",NULL);
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		gtk_text_buffer_insert_with_tags_by_name(chat_history_buffer,&end,nick,strlen(nick),"bold",NULL);
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		if(id!=-1)
			gtk_text_buffer_insert_with_tags_by_name(chat_history_buffer,&end,">",1,"blue",NULL);
		else
			gtk_text_buffer_insert_with_tags_by_name(chat_history_buffer,&end,">",1,"purple",NULL);
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		gtk_text_buffer_insert(chat_history_buffer,&end," ",1);
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		gtk_text_buffer_insert(chat_history_buffer,&end,msg,len);
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		gtk_text_buffer_insert(chat_history_buffer,&end,"\n",1);
		chat_scroll_to_end();
	}
	else
		return;
}

void chat_add_action(int32_t id, char *action, int32_t len)
{
	char *nick = client_get_name(id);
	if(nick)
	{
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		if(id!=-1)
			gtk_text_buffer_insert_with_tags_by_name(chat_history_buffer,&end,"*** ",4,"blue",NULL);
		else
			gtk_text_buffer_insert_with_tags_by_name(chat_history_buffer,&end,"*** ",4,"purple",NULL);
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		gtk_text_buffer_insert_with_tags_by_name(chat_history_buffer,&end,nick,strlen(nick),"bold",NULL);
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		gtk_text_buffer_insert(chat_history_buffer,&end," ",1);
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		gtk_text_buffer_insert(chat_history_buffer,&end,action,len);
		gtk_text_buffer_get_end_iter(chat_history_buffer,&end);
		gtk_text_buffer_insert(chat_history_buffer,&end,"\n",1);
		chat_scroll_to_end();
	}
	else
		return;
}
