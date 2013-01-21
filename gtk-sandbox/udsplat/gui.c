#include <gtk/gtk.h>
#include <string.h>
#include "gui.h"

/*
	Some globals
*/

GtkWidget *broadcast_ip, *scrollback, *message, *nickname, *scrollback_sv;

/*
	Callback Functions
*/

static gboolean gui_delete_event(GtkWidget *window, gpointer user_data)
{
	/*
		Could possibly add some sort of "Blah left the chat"
	*/
	
	gtk_main_quit();
	return FALSE;
}

static void gui_message_activate(GtkWidget *message_box, gpointer user_data)
/* FORM OF JANE'S TAMPON! */
{
	/* Don't be so verbose about no message, users oughta understand that */
	if(strlen((char*)gtk_entry_get_text(GTK_ENTRY(message_box)))<1)
		return;

	/* Make sure users realise they don't have a nickname */
	if(strlen((char*)gtk_entry_get_text(GTK_ENTRY(nickname)))<1)
	{
		printf("You need a nickname in order to send messages!\n");
		return;
	}
	network_send_message((char*)gtk_entry_get_text(GTK_ENTRY(nickname)), (char*)gtk_entry_get_text(GTK_ENTRY(message_box)));
	gtk_entry_set_text(GTK_ENTRY(message_box), "");
}

static void gui_scrollback_scroll_to_end()
{
	GtkAdjustment *spos = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrollback_sv));
	if(spos->value == spos->upper-spos->page_size)
	{
		GtkTextBuffer *scrollback_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(scrollback));
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(scrollback_buf,&end);
		GtkTextMark *pos = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(scrollback_buf),NULL,&end,0);
		gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(scrollback),pos,0,1,1,1);
	}
}

static void gui_broadcast_activate(GtkWidget *broadcast_text, gpointer user_data)
{
	if(!gethostbyname((char*)gtk_entry_get_text(GTK_ENTRY(broadcast_text))))
	{
		printf("Can't change IP address as it doesn't resolve!\n");
		return;
	}
	network_change_ip((char*)gtk_entry_get_text(GTK_ENTRY(broadcast_text)));
	gtk_widget_grab_focus(message);
}

/*
	Basic Run Down Of GTK+ Syntax

	Functions look like:
	gtk_<widget_name>_<function_name>(...);

	Datatypes look like:
	GtkSomeDataType *object;

	Constants look like:
	GTK_<WIDGET_NAME>_<DETAIL>

	Also, you create many different types of widgets as 'GtkWidget',
	which is a base 'class' for other widgets. This causes a problem
	since there is no polymorphism in C, so, if you were to do:

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_fullscreen(window, TRUE);

	You would get a warning, because gtk_window_set_fullscreen, expects
	a GtkWindow, not a GtkWidget. So instead you do:

	gtk_window_set_fullscreen(GTK_WINDOW(window), TRUE);

	You could argue you should just cast to GTK_WIDGET, when needed, but
	I can't remember / don't know why this isn't done.
*/

void gui_init(void)
{
	/*
		______________________________________________
		|Nickname:[_Feesh____] IP: [255.255.255.255_]|
		||------------------------------------------||
		|| Scrollback Buffer                        ||
		||                                          ||
		||                                          ||
		||                                          ||
		||                                          ||
		||                                          ||
		||                                          ||
		||__________________________________________||
		|[ Message                                  ]|
		|____________________________________________|
	*/

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window, 500, 400);
	GtkWidget *hbox, *vbox;

	broadcast_ip = gtk_entry_new_with_max_length(16);
	gtk_entry_set_text(GTK_ENTRY(broadcast_ip),"255.255.255.255");

	nickname = gtk_entry_new_with_max_length(16);
	gtk_entry_set_text(GTK_ENTRY(nickname),"Anon");
	message = gtk_entry_new();
	gtk_widget_modify_font(message, pango_font_description_from_string("courier 10"));
	scrollback = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(scrollback), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(scrollback), GTK_WRAP_WORD_CHAR);
	gtk_widget_modify_font(scrollback, pango_font_description_from_string("courier 10"));
	/*
		Create a formatting tag, so nicknames will be bold,
		along with info messages.
	*/	
	GtkTextBuffer *scrollback_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(scrollback));
	gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(scrollback_buf),"bold","weight","bold",NULL);

	/*
		In order to make the scrollback buffer scrollable (and generally
		pleasant), I use a GtkScrolledWindow as an adapter.
	*/
	scrollback_sv = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrollback_sv),GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollback_sv),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrollback_sv), scrollback);

	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Nickname: "),FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox),nickname,FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Broadcast IP: "),FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox),broadcast_ip,FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), hbox,FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), scrollback_sv, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), message,FALSE, FALSE, 0);

	gtk_widget_grab_focus(message);
	
	/*
		Hookup callbacks
	*/

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gui_delete_event), NULL);
	g_signal_connect(G_OBJECT(message), "activate", G_CALLBACK(gui_message_activate), NULL);
	g_signal_connect(G_OBJECT(broadcast_ip),"activate", G_CALLBACK(gui_broadcast_activate), NULL);
	gui_scrollback_add(NULL, "~~~ Welcome to UDsPlat ~~~\n");

	gtk_widget_show_all(window);
}

void gui_scrollback_add(char *who, char *message)
{
	/*
		Function used to add messages to the scrollback buffer.
		Will automatically add a \n at the end for good measure.
	*/
	if(!scrollback)
	{
		/* Woah, hold your horses, the text view hasn't even been created */
		return;
	}

	GtkTextBuffer *scrollback_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(scrollback));
	GtkTextIter end;
	
	gtk_text_buffer_get_end_iter(scrollback_buf, &end);
	if(who!=NULL)
	{
		gtk_text_buffer_insert_with_tags_by_name(scrollback_buf, &end, who, -1, "bold",NULL);
		gtk_text_buffer_get_end_iter(scrollback_buf, &end);
		gtk_text_buffer_insert(scrollback_buf, &end, ": ", 2);
		gtk_text_buffer_get_end_iter(scrollback_buf, &end);
		gtk_text_buffer_insert(scrollback_buf, &end, message, -1);
	}
	else
	{
		gtk_text_buffer_insert_with_tags_by_name(scrollback_buf, &end, message, -1, "bold",NULL);
	}
	gtk_text_buffer_get_end_iter(scrollback_buf, &end);
	gtk_text_buffer_insert(scrollback_buf, &end, "\n", 1);
	
	gui_scrollback_scroll_to_end();
}
