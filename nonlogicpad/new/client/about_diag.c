#include <gtk/gtk.h>
#include "banner.h"

static GtkWidget *about_diag_create();
static gboolean about_diag_destroy(GtkWidget *window, gpointer user_data);

void about_diag_show()
{
	gtk_widget_show_all(about_diag_create());
}

GtkWidget *about_diag_create()
{
	GtkWidget *window = NULL, *table = NULL, *close_button = NULL, *label = NULL, *text_scroll = NULL, *text = NULL;
	GtkTextBuffer *text_buffer = NULL;
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(window),0);
	gtk_window_set_type_hint(GTK_WINDOW(window),GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_widget_set_size_request(window,350,400);
	
	label = banner_create("Nonlogic Pad - About",33);

	close_button = gtk_button_new_with_label("Close");

	text_scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(text_scroll),GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(text_scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	
	text = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text),GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text),0);
	text_buffer = gtk_text_buffer_new(NULL);

	gtk_text_buffer_set_text(text_buffer,
"~~Nonlogic Pad About~~\n\n\
Nonlogic Pad was originally created using Clanlib (www.clanlib.org) \
under the name of 'Techpad'. The idea came from a Skype three way \
call between RodgerTheGreat, Shadowarts and Feesh!. It was discussed \
that planning projects and designing things would be easier if there \
was a program that could allow people to draw collaboratively over \
the net.\n\nSo with a bit of talking about how it 'could' be done \
the conversation soon changed to another topic, however Feesh! \
started work on the first version of Techpad shortly after the \
call. It was very sensitive and had very few features. It did \
what it was supposed to do, but it didn't do it well.\n\n\
Fast forward a year and Techpad starts getting some more attention \
but it just isn't easy enough to get everyone set up with the \
Clanlib libraries, so Feesh! starts to rebuild Techpad under the new \
name of Techpad 2, which shortly became Nonlogic Pad.\n\n\
Nonlogic Pad originally used Cairo for it's drawing backend but due \
to demand for faster drawing and flood filling, Cairo was dropped \
for GTK's graphical backend, GDK.\n\n\
Nonlogic Pad now boasts a wide range of features and is easily extended \
upon. It has been a persistant project that will probably be getting \
upgraded many times in the future when the need is great enough.\n\n\
~~Thanks to:~~\n\n\
Shadowarts and RodgerTheGreat for the original idea and features\n\
ChuckFromChan for instigating the creation of the new Nonlogic Pad\n\
Gwildor for feature ideas and testing\
\nEveryone else at Nonlogic that took the time to have a go of Nonlogic Pad.\n\
"	,-1);

	gtk_container_add(GTK_CONTAINER(text_scroll),text);

	gtk_text_view_set_buffer(GTK_TEXT_VIEW(text),text_buffer);

	table = gtk_table_new(5,3,0);
	
	gtk_table_attach(GTK_TABLE(table),label,0,5,0,1,GTK_FILL,0,5,2);
	gtk_table_attach(GTK_TABLE(table),text_scroll,0,5,1,2,GTK_EXPAND|GTK_FILL,GTK_EXPAND|GTK_FILL,5,0);
	gtk_table_attach(GTK_TABLE(table),close_button,0,1,2,3,GTK_FILL,0,0,0);

	gtk_container_add(GTK_CONTAINER(window),table);

	g_signal_connect(G_OBJECT(window),"destroy-event",G_CALLBACK(about_diag_destroy),NULL);
	g_signal_connect_swapped(G_OBJECT(close_button),"clicked",G_CALLBACK(about_diag_destroy),window);
	return window;
}


gboolean about_diag_destroy(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	return 0;
}
