#include <gtk/gtk.h>

GtkWidget *tb_select, *tb_rotate, *tb_scale, *tb_line, *tb_square, *tb_circle,*tb_triangle, *tb_clear;

GtkWidget *toolbox_create()
{
	GtkWidget *toolbox = NULL;
	
	toolbox = gtk_table_new(5,2,1);
	
	tb_select = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(tb_select),gtk_image_new_from_file("icons/select.png"));
	
	tb_rotate = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(tb_rotate),gtk_image_new_from_file("icons/rotate.png"));
	
	tb_scale = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(tb_scale),gtk_image_new_from_file("icons/scale.png"));
	
	tb_line = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(tb_line),gtk_image_new_from_file("icons/line.png"));
	
	tb_square = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(tb_square),gtk_image_new_from_file("icons/square.png"));
	
	tb_circle = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(tb_circle),gtk_image_new_from_file("icons/circle.png"));
	
	tb_triangle = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(tb_triangle),gtk_image_new_from_file("icons/triangle.png"));
	
	tb_clear = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(tb_clear),gtk_image_new_from_file("icons/clear.png"));
	
	gtk_table_attach(GTK_TABLE(toolbox),tb_select,0,1,0,1,0,0,0,0);
	gtk_table_attach(GTK_TABLE(toolbox),tb_scale,1,2,0,1,0,0,0,0);
	gtk_table_attach(GTK_TABLE(toolbox),tb_rotate,0,1,1,2,0,0,0,0);
	gtk_table_attach(GTK_TABLE(toolbox),tb_line,1,2,1,2,0,0,0,0);
	gtk_table_attach(GTK_TABLE(toolbox),tb_square,0,1,2,3,0,0,0,0);
	gtk_table_attach(GTK_TABLE(toolbox),tb_circle,1,2,2,3,0,0,0,0);
	gtk_table_attach(GTK_TABLE(toolbox),tb_triangle,0,1,3,4,0,0,0,0);
	gtk_table_attach(GTK_TABLE(toolbox),tb_clear,1,2,3,4,0,0,0,0);
	

	return toolbox;
}
