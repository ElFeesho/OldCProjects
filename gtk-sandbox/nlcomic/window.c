#include <gtk/gtk.h>
#include "menu.h"
#include "panels.h"

/* Prototypes */
static gboolean window_destroy(GtkWidget *window, gpointer user_data);
static GtkWidget *window_create();

static GtkWidget *window_create()
{
	GtkWidget	*window = NULL, 
					*table = NULL, 
					*tools_select = NULL, 
					*tools_text = NULL, 
					*tools_object = NULL, 
					*tools_scale = NULL, 
					*tools_rotate = NULL, 
					*object_x = NULL,
					*object_y = NULL,
					*object_rot = NULL,
					*object_scl = NULL,
					*object_emt = NULL,
					*object_act = NULL,					
					*object_dir = NULL,
					*panel_add = NULL, 
					*panel_rem = NULL;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window),500,400);
	table = gtk_table_new(3,17,0);
	
	/* Make Tool Buttons */

	tools_select = gtk_button_new_with_label("Select");
	tools_scale = gtk_button_new_with_label("Scale");
	tools_rotate = gtk_button_new_with_label("Rotate");
	tools_text = gtk_button_new_with_label("Text");
	tools_object = gtk_button_new_with_label("Object");

	/* Object Data Widgets */

	object_x = gtk_entry_new();
	object_y = gtk_entry_new();
	object_rot = gtk_entry_new();
	object_scl = gtk_entry_new();
	object_emt = gtk_entry_new();
	object_act = gtk_entry_new();
	object_dir = gtk_entry_new();

	gtk_widget_set_size_request(object_x,50,20);
	gtk_widget_set_size_request(object_y,50,20);
	gtk_widget_set_size_request(object_rot,50,20);
	gtk_widget_set_size_request(object_scl,50,20);
	gtk_widget_set_size_request(object_emt,50,20);
	gtk_widget_set_size_request(object_act,50,20);
	gtk_widget_set_size_request(object_dir,50,20);

	/* Make Panel Buttons */
	
	panel_add = gtk_button_new_with_label("Add");
	panel_rem = gtk_button_new_with_label("Remove");

	/* Attach Widgets! */
	
	gtk_table_attach(GTK_TABLE(table),menu_create(),0,3,0,1,GTK_FILL|GTK_EXPAND,0,0,0);
	gtk_table_attach(GTK_TABLE(table),panels_create(),0,1,1,17,GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Tools"),1,3,1,2,GTK_FILL|GTK_EXPAND,0,0,0);
	gtk_table_attach(GTK_TABLE(table),tools_select,1,2,2,3,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),tools_object,2,3,2,3,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),tools_text,1,2,3,4,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),tools_scale,2,3,3,4,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),tools_rotate,1,2,4,5,GTK_FILL,0,0,0);
	
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Object Options"),1,3,5,6,GTK_FILL|GTK_EXPAND,0,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("X:"),1,2,6,7,GTK_EXPAND|GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Y:"),1,2,7,8,GTK_EXPAND|GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Scale:"),1,2,8,9,GTK_EXPAND|GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Rot.:"),1,2,9,10,GTK_EXPAND|GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Emt:"),1,2,10,11,GTK_EXPAND|GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Action:"),1,2,11,12,GTK_EXPAND|GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Dir:"),1,2,12,13,GTK_EXPAND|GTK_FILL,0,0,0);

	gtk_table_attach(GTK_TABLE(table),object_x,2,3,6,7,0,0,0,0);
	gtk_table_attach(GTK_TABLE(table),object_y,2,3,7,8,0,0,0,0);
	gtk_table_attach(GTK_TABLE(table),object_scl,2,3,8,9,0,0,0,0);
	gtk_table_attach(GTK_TABLE(table),object_rot,2,3,9,10,0,0,0,0);
	gtk_table_attach(GTK_TABLE(table),object_emt,2,3,10,11,0,0,0,0);
	gtk_table_attach(GTK_TABLE(table),object_act,2,3,11,12,0,0,0,0);
	gtk_table_attach(GTK_TABLE(table),object_dir,2,3,12,13,0,0,0,0);

	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Panels"),1,3,13,14,GTK_EXPAND|GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),panel_add,1,2,14,15,GTK_EXPAND|GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),panel_rem,2,3,14,15,GTK_EXPAND|GTK_FILL,0,0,0);
	panels_add();

	/* Oh and lets put the table in the window, there's a good chap */

	gtk_container_add(GTK_CONTAINER(window),table);		

	/* Attach Callbacks! */
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(window_destroy),NULL);

	return window;	
}

void window_show()
{
	gtk_widget_show_all(window_create());
}

gboolean window_destroy(GtkWidget *window, gpointer user_data)
{
	gtk_exit(0);
	return 0;
}
