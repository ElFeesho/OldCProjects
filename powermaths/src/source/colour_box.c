#include <gtk/gtk.h>

static GtkWidget *colour_box_gen(char *colour)
{
	GtkWidget *ev_box = gtk_event_box_new();
	GdkColor col;
	gdk_color_parse(colour,&col);
	gtk_widget_modify_bg(ev_box,GTK_STATE_NORMAL,&col);
	return ev_box;
}

GtkWidget *colour_box_create()
{
	GtkWidget *colour_box = gtk_table_new(2,10,1);
	
	GtkWidget *ev_box[16];
	
	ev_box[0] = colour_box_gen("#000");
	ev_box[1] = colour_box_gen("#f00");
	ev_box[2] = colour_box_gen("#0f0");
	ev_box[3] = colour_box_gen("#00f");
	ev_box[4] = colour_box_gen("#ff0");
	ev_box[5] = colour_box_gen("#0ff");
	ev_box[6] = colour_box_gen("#f0f");
	ev_box[7] = colour_box_gen("#fff");
	ev_box[8] = colour_box_gen("#666");
	ev_box[9] = colour_box_gen("#900");
	ev_box[10] = colour_box_gen("#090");
	ev_box[11] = colour_box_gen("#009");
	ev_box[12] = colour_box_gen("#990");
	ev_box[13] = colour_box_gen("#099");
	ev_box[14] = colour_box_gen("#909");
	ev_box[15] = colour_box_gen("#999");
	
	gtk_widget_set_size_request(ev_box[0],24,24);
	int i;
	for(i=0;i<16;i++)
	{
		if(i<8)
		{
			gtk_table_attach_defaults(GTK_TABLE(colour_box),ev_box[i],i,i+1,0,1);
		}
		else
		{
			gtk_table_attach_defaults(GTK_TABLE(colour_box),ev_box[i],i-8,i-7,1,2);
		}
	}
	
	return colour_box;
}
