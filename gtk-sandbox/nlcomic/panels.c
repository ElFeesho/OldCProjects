#include <gtk/gtk.h>
#include "canvas.h"

#define MAX_PANELS 16

static GtkWidget *panels = 0;

int panels_default_w = 300;
int panels_default_h = 300;

GtkWidget *panels_create()
{
	panels = gtk_notebook_new();
	return panels;
}

int panels_add()
{
	if(gtk_notebook_get_n_pages(GTK_NOTEBOOK(panels))<MAX_PANELS)
	{
		GtkWidget *canvas = canvas_create(gtk_notebook_get_n_pages(GTK_NOTEBOOK(panels)));
		gtk_widget_set_size_request(canvas,panels_default_w, panels_default_h);
		gtk_notebook_append_page(GTK_NOTEBOOK(panels),canvas,gtk_label_new((char*)g_strdup_printf("Panel %d",gtk_notebook_get_n_pages(GTK_NOTEBOOK(panels))+1)));
	}
	return gtk_notebook_get_n_pages(GTK_NOTEBOOK(panels));
}

int panels_remove()
{
	gtk_notebook_remove_page(GTK_NOTEBOOK(panels),gtk_notebook_get_current_page(GTK_NOTEBOOK(panels)));
   return gtk_notebook_get_n_pages(GTK_NOTEBOOK(panels));
}

void panels_update()
{
	/* Run through the panels and set the child widget's size request to the new size */
}
