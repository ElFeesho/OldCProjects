#define WIDTH 640
#define HEIGHT 480
#include <gtk/gtk.h>

#include "camera.h"
#include "network.h"
#include "manipulate.h"

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

static void save_image(GtkWidget *webcam)
{
	static int file_num = 0;
	int w, h;
   gdk_drawable_get_size(webcam->window,&w, &h);
   GdkPixbuf *dump = gdk_pixbuf_get_from_drawable(NULL,webcam->window,NULL,0,0,0,0,w, h);
   gdk_pixbuf_save(dump,g_strdup_printf("DUMP%.04d.png",file_num++),"png",NULL,"compression","9",NULL);
}

static gboolean update_camera(GtkWidget *webcam)
{
	unsigned char *data = camera_read_image();
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
	gdk_draw_rgb_image(webcam->window, gc,0,0,WIDTH, HEIGHT ,GDK_RGB_DITHER_NONE,(unsigned char*)data,WIDTH*3);
	if(x!=-2)
	{
//		gdk_draw_rectangle(webcam->window, gc, 0,x,y,x2-x,y2-y);
		//save_image(webcam);	
	}
	return TRUE;
}

void gain_changed(GtkRange *range, gpointer user_data)
{
	double value = gtk_range_get_value(range);
	camera_set_gain((int)value);
}

void whitebalance_red_changed(GtkRange *range, gpointer user_data)
{
	double value = gtk_range_get_value(range);
	camera_set_white_balance_red(value);
}

void whitebalance_blue_changed(GtkRange *range, gpointer user_data)
{
	double value = gtk_range_get_value(range);
	camera_set_white_balance_blue(value);
}

void balance_red_changed(GtkRange *range, gpointer user_data)
{
	double value = gtk_range_get_value(range);
	camera_set_red_balance(value);
}

void balance_blue_changed(GtkRange *range, gpointer user_data)
{
	double value = gtk_range_get_value(range);
	camera_set_blue_balance(value);
}

void exposure_changed(GtkRange *range, gpointer user_data)
{
	double value = gtk_range_get_value(range);
	camera_set_exposure(value);
}

void gui_init()
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *table = gtk_table_new(7,3,0);
	GtkWidget *camera_win = gtk_drawing_area_new();
	GtkWidget *gain = gtk_hscale_new_with_range(0.0,63,1);
	
	GtkWidget *whitebalance_red = gtk_hscale_new_with_range(0.0,63,1);
	GtkWidget *whitebalance_blue = gtk_hscale_new_with_range(0.0,63,1);
	GtkWidget *red_balance = gtk_hscale_new_with_range(0.0,127,1);
	GtkWidget *blue_balance = gtk_hscale_new_with_range(0.0,127,1);
	GtkWidget *exposure = gtk_hscale_new_with_range(0.0,255,1);
	gtk_widget_set_size_request(exposure, 100,0);
	
	gtk_widget_set_size_request(camera_win, WIDTH, HEIGHT);
	
	gtk_table_attach_defaults(GTK_TABLE(table),camera_win,0,1,0,7);
	
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Exposure:"),1,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table),exposure,2,3,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Gain:"),1,2,2,3);
	gtk_table_attach_defaults(GTK_TABLE(table),gain,2,3,2,3);

	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("White Balance Red:"),1,2,3,4);
	gtk_table_attach_defaults(GTK_TABLE(table),whitebalance_red,2,3,3,4);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("White Balance Blue:"),1,2,4,5);
	gtk_table_attach_defaults(GTK_TABLE(table),whitebalance_blue,2,3,4,5);

	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Red Balance:"),1,2,5,6);
	gtk_table_attach_defaults(GTK_TABLE(table),red_balance,2,3,5,6);
	gtk_table_attach_defaults(GTK_TABLE(table),gtk_label_new("Blue Balance:"),1,2,6,7);
	gtk_table_attach_defaults(GTK_TABLE(table),blue_balance,2,3,6,7);

	gtk_container_add(GTK_CONTAINER(window), table);	
	g_signal_connect(G_OBJECT(gain),"value-changed", G_CALLBACK(gain_changed),NULL);
	g_signal_connect(G_OBJECT(exposure),"value-changed", G_CALLBACK(exposure_changed),NULL);
	g_signal_connect(G_OBJECT(whitebalance_red),"value-changed", G_CALLBACK(whitebalance_red_changed),NULL);
	g_signal_connect(G_OBJECT(whitebalance_blue),"value-changed", G_CALLBACK(whitebalance_blue_changed),NULL);
	g_signal_connect(G_OBJECT(red_balance),"value-changed", G_CALLBACK(balance_red_changed),NULL);
	g_signal_connect(G_OBJECT(blue_balance),"value-changed", G_CALLBACK(balance_blue_changed),NULL);



	g_signal_connect(G_OBJECT(window),"delete-event", G_CALLBACK(window_destroy), NULL);
	g_idle_add((GSourceFunc)update_camera, camera_win);
	gtk_widget_show_all(window);
}
