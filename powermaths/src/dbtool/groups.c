#include <gtk/gtk.h>
#include "groups.h"

static int groups_selected = -1;

extern GtkWidget *groups_create()
{
	GtkWidget *viewport, *table, *combo_box;
	
	viewport = gtk_viewport_new(NULL,NULL);
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(viewport),GTK_SHADOW_NONE);
	
	table = gtk_table_new(9,9,FALSE);
	
	gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Select a group you wish to add questions into."),0,9,0,1);
	
	gtk_container_add(GTK_CONTAINER(viewport),table);
	
	return viewport;
}

extern int groups_get_id()
{
	return groups_selected;
}	
