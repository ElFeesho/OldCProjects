#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "main_window.h"

void image_dump()
{
	int w, h;
	gdk_drawable_get_size(main_window_canvas_win,&w, &h);
	GdkPixbuf *dump = gdk_pixbuf_get_from_drawable(NULL,main_window_canvas_win,NULL,0,0,0,0,w, h);

	GtkWidget *save_diag = gtk_file_chooser_dialog_new("Save Dump As",NULL,GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,NULL);

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.png");
	gtk_file_filter_set_name(filter, "PNG (*.png)");
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(save_diag),filter);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(save_diag), TRUE);

	if(gtk_dialog_run(GTK_DIALOG(save_diag)) == GTK_RESPONSE_ACCEPT)
	{
		const char *fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(save_diag));
		char *sfname = NULL;
		if(strlen(fname)<=3)
			sfname = g_strdup_printf("%s.png",fname);
		else
		{
			if(strcmp(fname+strlen(fname)-4,".png")!=0)
			{
				sfname = g_strdup_printf("%s.png",fname);
			}
			else
			{
				sfname = g_strdup_printf("%s",fname);
			}
		}
		gdk_pixbuf_save(dump,sfname,"png",NULL,"compression","9",NULL);	 
		g_free(sfname);
	}
	gtk_widget_destroy(save_diag);
}
