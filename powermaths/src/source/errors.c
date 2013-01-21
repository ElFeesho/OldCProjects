#include "errors.h"

/* Tumble weed... */

void error_dialog(char *message)
{
	//Error dialog creator
	GtkDialog *error_dlg = GTK_DIALOG(gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,message));
	gtk_dialog_run(error_dlg);
	gtk_widget_destroy(GTK_WIDGET(error_dlg));
}
