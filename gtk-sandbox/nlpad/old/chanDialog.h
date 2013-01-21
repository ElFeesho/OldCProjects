#ifndef __CHANDLG_H__
#define __CHANDLG_H__

#include <gtk/gtk.h>

typedef struct _chanDlg
{
	GtkWidget *window;
	GtkWidget *channel;
	GtkWidget *table;
	GtkWidget *ok, *cancel;
} chanDlg;

chanDlg chanDLG;


//Create dialog
void makeChannelDialog();

//Callbacks
void onChannelOk(GtkWidget *widget, gpointer user_data);
void onChannelCancel(GtkWidget *widget, gpointer user_data);

#endif
