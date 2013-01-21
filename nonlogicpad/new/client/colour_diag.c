#include <gtk/gtk.h>
#include "globals.h"

/* Callback Declarations */

static void colour_diag_ok_clicked(GtkWidget *widget, gpointer user_data);

/* End of Callback Declarations */

static GtkWidget *colour_diag_init()
{
	GdkColor colour;
	colour.red = globals_red;
	colour.green = globals_green;
	colour.blue = globals_blue;
   
	GtkWidget *window = gtk_color_selection_dialog_new("Choose an ink Colour");

	gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(((GtkColorSelectionDialog*)window)->colorsel),&colour);
	
	g_signal_connect(G_OBJECT(((GtkColorSelectionDialog*)window)->ok_button),"clicked",G_CALLBACK(colour_diag_ok_clicked),window);
	g_signal_connect_swapped(G_OBJECT(((GtkColorSelectionDialog*)window)->cancel_button),"clicked",G_CALLBACK(gtk_widget_destroy),window);
	g_signal_connect_swapped(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_widget_destroy),window);
	return window;
}

void colour_diag_show()
{
	gtk_widget_show_all(colour_diag_init());
}

/* Callback Declarations */

void colour_diag_ok_clicked(GtkWidget *widget, gpointer user_data)
{
	GdkColor ncol;
	gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(((GtkColorSelectionDialog*)user_data)->colorsel),&ncol);
	globals_red = ncol.red;
	globals_green = ncol.green;
	globals_blue = ncol.blue;
	gtk_widget_destroy(user_data);
}

/* End of Callback Declarations */
