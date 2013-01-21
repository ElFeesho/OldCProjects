#include <gtk/gtk.h>
#include "window.h"

static GtkWidget *table = NULL;

GtkWidget *toolopts_create()
{
	table = gtk_table_new(5,3,0);
	gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Line Tool"),0,3,0,1);
	return table;
}

void toolopts_change_tool(int toolmode)
{
	gtk_container_foreach(GTK_CONTAINER(table),(GtkCallback)(gtk_widget_destroy),NULL);
	if(toolmode == TOOL_LINE)
	{
		gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Line Tool"),0,3,0,1);
	}
	if(toolmode == TOOL_RECT)
	{	
		gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Rectangle Tool"),0,3,0,1);
	}
	if(toolmode == TOOL_CIRC)
	{	
		gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Circle Tool"),0,3,0,1);
	}
	if(toolmode == TOOL_TRI)
	{	
		gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Triangle Tool"),0,3,0,1);
	}
	if(toolmode == TOOL_SELECT)
	{	
		gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Select Tool"),0,3,0,1);
	}
	if(toolmode == TOOL_SCALE)
	{	
		gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Scale Tool"),0,3,0,1);
	}
	if(toolmode == TOOL_ROTATE)
	{	
		gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Rotate Tool"),0,3,0,1);
	}
	gtk_widget_show_all(table);
}

