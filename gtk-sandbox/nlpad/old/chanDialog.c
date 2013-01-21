#include <string.h>
#include "chanDialog.h"
#include "globals.h"
#include "helper.h"

//Create dialog
void makeChannelDialog()
{
	chanDLG.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(chanDLG.window),"Change Channel");
	
	chanDLG.table = gtk_table_new(3,2,0);
	gtk_container_add(GTK_CONTAINER(chanDLG.window),chanDLG.table);

	chanDLG.channel = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(chanDLG.channel),globals.channel);
	chanDLG.ok = gtk_button_new_with_label("Ok");
	chanDLG.cancel = gtk_button_new_with_label("Cancel");

	gtk_table_attach_defaults(GTK_TABLE(chanDLG.table),gtk_label_new("Channel:"),0,1,0,1);
	gtk_table_attach_defaults(GTK_TABLE(chanDLG.table),chanDLG.channel,1,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(chanDLG.table),chanDLG.ok,0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(chanDLG.table),chanDLG.cancel,1,2,1,2);
	
	g_signal_connect(G_OBJECT(chanDLG.ok),"clicked",G_CALLBACK(onChannelOk),NULL);
	g_signal_connect(G_OBJECT(chanDLG.cancel),"clicked",G_CALLBACK(onChannelCancel),NULL);
	
}

//Callbacks
void onChannelOk(GtkWidget *widget, gpointer user_data)
{
	if(strcmp(globals.channel,(char*)gtk_entry_get_text(GTK_ENTRY(chanDLG.channel)))==0)
	{
		gtk_widget_hide(chanDLG.window);
		return;
	}
	strcpy(globals.channel,gtk_entry_get_text(GTK_ENTRY(chanDLG.channel)));
	changeChannel();
	gtk_widget_hide(chanDLG.window);
}

void onChannelCancel(GtkWidget *widget, gpointer user_data)
{
	gtk_widget_hide(chanDLG.window);
}
