#include <gtk/gtk.h>
#include <stdlib.h>
#include "groups.h"
#include "database.h"
#include "errors.h"

void splash_show()
{
	//Show splash here
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *splash_img = gtk_image_new_from_file("pixmaps/splash.png");

	gtk_widget_set_size_request(window,500,300);
	gtk_window_set_decorated(GTK_WINDOW(window),0);
	gtk_window_set_keep_above(GTK_WINDOW(window),1);
	gtk_container_add(GTK_CONTAINER(window),splash_img);
	gtk_widget_show_all(window);
	
	switch(database_open("questions.db"))
	{
		case -1:
			error_dialog("Failed to open questions database, closing!");
			gtk_exit(1);
			break;
		case -2:
			error_dialog("Database isn't setup correctly!");
			gtk_exit(1);
			break;
	}

	int i = 0;
	if(!groups_load_list())
	{	
		error_dialog("Couldn't load the groups!");
		exit(1);
	}
	for(;i<100;i++)
	{
		while(gtk_events_pending())
			gtk_main_iteration();
		//usleep(50000);		
		//sleep(1);
	}
	gtk_widget_hide(window);
	gtk_widget_destroy(window);
}
