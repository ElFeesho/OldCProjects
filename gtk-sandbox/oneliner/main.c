#include <gtk/gtk.h>

extern gchar *strip_lines_tabs(gchar *string);

GtkWidget *ml_text, *sl_text;

static void convert_text(GtkWidget *button, gpointer user_data)
{
	GtkTextBuffer *text_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ml_text));

	GtkTextIter start, end;

	gtk_text_buffer_get_bounds(text_buf, &start, &end);
	gchar *buffer_text = gtk_text_buffer_get_text(text_buf,&start, &end,1);
	gchar *altered_text = strip_lines_tabs(buffer_text);
	
	gtk_entry_set_text(GTK_ENTRY(sl_text), altered_text);
	
}

int main(int argc, char **argv)
{
	/* 
		Gui is gonna look like...
		
		-------------------------------
		| ___________________________ |
		||                           ||
		||                           ||
		||                           ||
		||                           ||
		||                           ||
		||___________________________||
		|-----------------------------|
		|-----------------------------|
		|[Convert]                    |
		-------------------------------
	*/
	
	gtk_init(&argc, &argv);

	GtkWidget *window, *scr_window, *convert, *vbox;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window, 480,320);
	
	ml_text = gtk_text_view_new();
	sl_text = gtk_entry_new();

	scr_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scr_window),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scr_window),GTK_SHADOW_IN);

	convert = gtk_button_new_with_label("Convert");

	vbox = gtk_vbox_new(FALSE,0);

	gtk_container_add(GTK_CONTAINER(scr_window),ml_text);

	gtk_box_pack_start(GTK_BOX(vbox),scr_window,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),sl_text,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),convert,FALSE,TRUE,0);
	
	gtk_container_add(GTK_CONTAINER(window),vbox);

	g_signal_connect(G_OBJECT(convert),"clicked",G_CALLBACK(convert_text),NULL);

	/* Bad bad bad, don't do this :( */
	g_signal_connect(G_OBJECT(window),"delete-event", G_CALLBACK(gtk_main_quit),NULL);

	gtk_widget_show_all(window);

	gtk_main();
	
	return 0;
}
