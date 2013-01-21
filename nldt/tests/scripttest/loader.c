#include <gtk/gtk.h>
#include "gizmos.h"

void loader_show()
{
	GtkWidget *loader = gtk_file_chooser_dialog_new("Open .lua File", NULL,GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
	int res = gtk_dialog_run(GTK_DIALOG(loader));
	if(res == GTK_RESPONSE_CANCEL)
	{
		gtk_exit(0);
	}
	if(res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;

		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(loader));
		
		gtk_widget_destroy(loader);
		nldt_gizmo_load(filename);
	}
}
