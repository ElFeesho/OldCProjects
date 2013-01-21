#define WIDTH 640
#define HEIGHT 480
#include <gtk/gtk.h>
#include <stdlib.h>

#include "camera.h"
#include "network.h"

/*
	Callbacks
*/

gboolean do_conv = FALSE;

static gboolean window_destroy(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	camera_shutdown();
	gtk_main_quit();
	return FALSE;
}

static void toggle_conv(GtkWidget *button, gpointer user_data)
{
	do_conv ^= 1;
}

static void save_image(GtkWidget *webcam)
{
	static int ltime = 0;
	if(ltime == 0)
	{
		ltime = time(NULL);
	}
	else if(time(NULL)-3<ltime)
	{
		return;
	}
	static int file_num = 0;
	unsigned int w, h;
   gdk_drawable_get_size(webcam->window,&w, &h);
   GdkPixbuf *dump = gdk_pixbuf_get_from_drawable(NULL,webcam->window,NULL,0,0,0,0,w, h);
	char *filename = g_strdup_printf("DUMP%d.png",time(NULL));
   gdk_pixbuf_save(dump,filename,"png",NULL,"compression","9",NULL);
	free(filename);
	ltime = time(NULL);
}

static gboolean update_camera(GtkWidget *webcam)
{
	char *data = camera_read_image();
	if(data==NULL)
		return TRUE;
	int x, y,x2,y2;
	manip_check_diffs(data, &x, &y, &x2, &y2);
	if(do_conv)
		convolute(data);
	unsigned char *send_data = NULL;
	int bytes = network_encode_data(data, &send_data);
	g_free(send_data);
	GdkGC *gc = gdk_gc_new(webcam->window);
	gdk_gc_set_line_attributes(gc, 2,GDK_LINE_ON_OFF_DASH,GDK_CAP_ROUND,GDK_JOIN_MITER);
	GdkColor red;
	gdk_color_parse("red",&red);
	gdk_gc_set_rgb_fg_color(gc,&red);
	gdk_draw_rgb_image(webcam->window, gc,0,0,WIDTH, HEIGHT ,GDK_RGB_DITHER_NONE,data,WIDTH*3);
	if(x!=-2)
	{
		gdk_draw_rectangle(webcam->window, gc, 0,x,y,x2-x,y2-y);
//		save_image(webcam);	
	}
	return TRUE;
}

void brightness_changed(GtkRange *range, gpointer user_data)
{
	double value = gtk_range_get_value(range);
	camera_set_brightness(value*40);
}

void contrast_changed(GtkRange *range, gpointer user_data)
{
	double value = gtk_range_get_value(range);
	camera_set_contrast(value*40);
}

void gui_init()
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *table = gtk_table_new(7,3,0);
	GtkWidget *conv = gtk_toggle_button_new_with_label("Convolute");
	GtkWidget *take_img = gtk_toggle_button_new_with_label("Snapshot");
	GtkWidget *camera_win = gtk_drawing_area_new();
	GtkWidget *brightness = gtk_hscale_new_with_range(0.0,1638,100);
	gtk_range_set_value(GTK_RANGE(brightness),800);
	gtk_widget_set_size_request(brightness,100,0);

	GtkWidget *contrast = gtk_hscale_new_with_range(0.0,1638,100);
	gtk_range_set_value(GTK_RANGE(contrast),800);
	
	gtk_widget_set_size_request(camera_win, WIDTH, HEIGHT);
	
	gtk_table_attach_defaults(GTK_TABLE(table),camera_win,0,1,0,7);
	
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Contrast"),1,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table),contrast,2,3,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Brightness"),1,2,2,3);
	gtk_table_attach_defaults(GTK_TABLE(table),brightness,2,3,2,3);
	gtk_table_attach_defaults(GTK_TABLE(table),conv,1,3,6,7);
	gtk_table_attach_defaults(GTK_TABLE(table),take_img,1,3,7,8);
	gtk_container_add(GTK_CONTAINER(window), table);	

	g_signal_connect(G_OBJECT(window),"delete-event", G_CALLBACK(window_destroy), NULL);
	g_signal_connect(G_OBJECT(conv),"clicked",G_CALLBACK(toggle_conv),NULL);
	g_signal_connect_swapped(G_OBJECT(take_img),"clicked",G_CALLBACK(save_image),camera_win);	
	g_signal_connect(G_OBJECT(contrast),"value-changed", G_CALLBACK(contrast_changed),NULL);
	g_signal_connect(G_OBJECT(brightness),"value-changed", G_CALLBACK(brightness_changed),NULL);

	g_idle_add((GSourceFunc)update_camera, camera_win);
	gtk_widget_show_all(window);
}
