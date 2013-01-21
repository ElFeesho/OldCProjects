#define WIDTH 320
#define HEIGHT 240
#include <gtk/gtk.h>

#include "camera.h"
#include "network.h"

/*
	Callbacks
*/

static gboolean window_destroy(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	camera_shutdown();
	gtk_main_quit();
	return FALSE;
}

static gboolean update_camera(GtkWidget *webcam)
{
	char *data = camera_read_image();
	if(data==NULL)
		return TRUE;
	int x, y,x2,y2;
	manip_check_diffs(data, &x, &y, &x2, &y2);
	unsigned char *send_data = NULL;
	int bytes = network_encode_data(data, &send_data);
	g_free(send_data);
	GdkGC *gc = gdk_gc_new(webcam->window);
	gdk_gc_set_line_attributes(gc, 2,GDK_LINE_ON_OFF_DASH,GDK_CAP_ROUND,GDK_JOIN_MITER);
	GdkColor red;
	gdk_color_parse("red",&red);
	gdk_gc_set_rgb_fg_color(gc,&red);
	gdk_draw_rgb_image(webcam->window, gc,0,0,WIDTH, HEIGHT ,GDK_RGB_DITHER_NONE,data,WIDTH*3);
	if(x!=-1)
		gdk_draw_rectangle(webcam->window, gc, 0,x,y,x2-x,y2-y);
	return TRUE;
}

void brightness_changed(GtkRange *range, gpointer user_data)
{
	double value = gtk_range_get_value(range);
	camera_set_gain(value);
}


void contrast_changed(GtkRange *range, gpointer user_data)
{
	double value = gtk_range_get_value(range);
	camera_set_red_balance(value);
}

void gui_init()
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *table = gtk_table_new(7,2,0);
	GtkWidget *camera_win = gtk_drawing_area_new();
	GtkWidget *brightness = gtk_hscale_new_with_range(0.0,1.0,0.01);
	gtk_range_set_value(GTK_RANGE(brightness),0.5);
	
	GtkWidget *contrast = gtk_hscale_new_with_range(0.0,1.0,0.01);
	gtk_range_set_value(GTK_RANGE(contrast),0.5);
	
	gtk_widget_set_size_request(camera_win, WIDTH, HEIGHT);
	
	gtk_table_attach_defaults(GTK_TABLE(table),camera_win,0,1,0,7);
	
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Contrast"),1,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table),contrast,1,2,1,2);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Brightness"),1,2,2,3);
	gtk_table_attach_defaults(GTK_TABLE(table),brightness,1,2,3,4);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Saturation"),1,2,5,6);
	gtk_container_add(GTK_CONTAINER(window), table);	
	g_signal_connect(G_OBJECT(brightness),"value-changed", G_CALLBACK(brightness_changed),NULL);
	g_signal_connect(G_OBJECT(contrast),"value-changed", G_CALLBACK(contrast_changed),NULL);
	g_signal_connect(G_OBJECT(window),"delete-event", G_CALLBACK(window_destroy), NULL);
	g_idle_add((GSourceFunc)update_camera, camera_win);
	gtk_widget_show_all(window);
}
