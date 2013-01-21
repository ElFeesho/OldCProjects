#include <gtk/gtk.h>
#include <stdio.h>
#include "main_window.h"

void image_dump()
{
	GdkPixbuf *dump = gdk_pixbuf_get_from_drawable(NULL,main_window_canvas_win,NULL,0,0,0,0,600,600);

	GtkWidget *save_diag = gtk_file_chooser_dialog_new("Save Dump As",NULL,GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,NULL);

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.png");
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(save_diag),filter);

	if(gtk_dialog_run(GTK_DIALOG(save_diag)) == GTK_RESPONSE_ACCEPT)
	{
		const char *fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(save_diag));
		gdk_pixbuf_save(dump,fname,"png",NULL,"compression","9",NULL);	 
	}
	gtk_widget_destroy(save_diag);

}
