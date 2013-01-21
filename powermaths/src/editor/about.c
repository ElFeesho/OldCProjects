#include <gtk/gtk.h>
#include "gtkvimage.h"

static GtkWidget *about_create()
{
	GtkWidget *window = NULL; 

	GtkWidget *vbox, *vimage, *hbox, *licence, *ok_button, *title, *desc;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window,300,300);
	gtk_window_set_resizable(GTK_WINDOW(window),0);
	
	vbox = gtk_vbox_new(FALSE,10);
	
	hbox = gtk_hbox_new(TRUE,50);
	
	title = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(title),"<span font_desc=\"36\"><b>Power<sup><i>Maths</i></sup></b></span>");

	vimage = gtk_vimage_new("pmlogo.svg");
	gtk_widget_set_size_request(vimage, 128,128);
	
	desc = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(desc),"<span><b>Power<sup><i>Maths</i></sup> Editor</b> is filmed in front of a live audience.</span>");
	
	ok_button = gtk_button_new_with_label("Ok");
	
	licence = gtk_button_new_with_label("Licence");
	
	gtk_box_pack_start(GTK_BOX(vbox),vimage,TRUE, TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),title,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),desc,FALSE, TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,TRUE,0);
	
	gtk_box_pack_start(GTK_BOX(hbox),ok_button,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbox),licence,FALSE,TRUE,0);
	
	gtk_container_add(GTK_CONTAINER(window),vbox);

	return window;
}

void about_show()
{
	GtkWidget *window = NULL;
	window = about_create();

	gtk_widget_show_all(window);
}
