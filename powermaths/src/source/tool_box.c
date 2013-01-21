#include <gtk/gtk.h>
#include "whiteb.h"

static const int WHITEB_FREEHAND	= 1;
static const int WHITEB_LINE		= 2;
static const int WHITEB_ERASER	= 3;
static const int WHITEB_RECT		= 4;
static const int WHITEB_CIRCLE	= 5;

GtkWidget *tool_box_create()
{
	GtkWidget *tool_box = gtk_table_new(10,2,1);
	
	GtkWidget *clear_board, *freeh_tool, *line_tool, *rect_tool, *circ_tool, *erase_tool, *exit_but;
	
	clear_board = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(clear_board),gtk_image_new_from_file("pixmaps/clear.png"));
	
	freeh_tool = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(freeh_tool),gtk_image_new_from_file("pixmaps/freeh.png"));
	
	line_tool = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(line_tool),gtk_image_new_from_file("pixmaps/line.png"));
	
	rect_tool = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(rect_tool),gtk_image_new_from_file("pixmaps/square.png"));	
	
	circ_tool = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(circ_tool),gtk_image_new_from_file("pixmaps/circle.png"));	
	
	erase_tool = gtk_button_new();	
	gtk_container_add(GTK_CONTAINER(erase_tool),gtk_image_new_from_file("pixmaps/eraser.png"));	

	exit_but = gtk_button_new_with_label("Exit");
	
	g_signal_connect(G_OBJECT(clear_board),"clicked",G_CALLBACK(whiteb_clear),NULL);
	g_signal_connect(G_OBJECT(freeh_tool),"clicked",G_CALLBACK(whiteb_change_tool),(void*)WHITEB_FREEHAND);
	g_signal_connect(G_OBJECT(line_tool),"clicked",G_CALLBACK(whiteb_change_tool),(void*)WHITEB_LINE);
	g_signal_connect(G_OBJECT(rect_tool),"clicked",G_CALLBACK(whiteb_change_tool),(void*)WHITEB_RECT);
	g_signal_connect(G_OBJECT(circ_tool),"clicked",G_CALLBACK(whiteb_change_tool),(void*)WHITEB_CIRCLE);
	g_signal_connect(G_OBJECT(erase_tool),"clicked",G_CALLBACK(whiteb_change_tool),(void*)WHITEB_ERASER);
	
	gtk_table_attach(GTK_TABLE(tool_box),clear_board,0,1,1,2,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_box),freeh_tool,0,1,2,3,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_box),line_tool,0,1,3,4,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_box),rect_tool,0,1,4,5,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_box),circ_tool,0,1,5,6,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_box),erase_tool,0,1,6,7,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_box),exit_but,0,1,7,8,GTK_FILL,0,0,0);

	return tool_box;
}
