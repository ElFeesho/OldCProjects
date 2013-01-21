#include <gtk/gtk.h>
#include <stdio.h>
#include "gtkpmbutton.h"

static void button_click(GtkWidget *button, gpointer user_data)
{
	printf("Button clicked\n");
}

static void button_hover(GtkWidget *button, gpointer user_data)
{
	printf("Button hovered\n");
}

static void button_leave(GtkWidget *button, gpointer user_data)
{
	printf("Button leaved\n");
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *button = gtk_pmbutton_new("Test Button");
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_exit),NULL);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(button_click),NULL);
	g_signal_connect(G_OBJECT(button),"leave",G_CALLBACK(button_leave),NULL);
	g_signal_connect(G_OBJECT(button),"enter",G_CALLBACK(button_hover),NULL);
	gtk_container_add(GTK_CONTAINER(window),button);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
