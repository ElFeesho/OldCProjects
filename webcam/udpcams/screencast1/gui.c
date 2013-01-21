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
	int x, y;
	unsigned char *send_data = NULL;
	int bytes = network_encode_data(data, &send_data);
	g_free(send_data);
	gdk_draw_rgb_image(webcam->window,gdk_gc_new(webcam->window),0,0,WIDTH, HEIGHT ,GDK_RGB_DITHER_NONE,data,WIDTH*3);
	return TRUE;
}

void gui_init()
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *camera_win = gtk_drawing_area_new();
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window),TRUE);
	gtk_widget_set_size_request(camera_win, WIDTH, HEIGHT);
	gtk_container_add(GTK_CONTAINER(window), camera_win);	
	g_signal_connect(G_OBJECT(window),"delete-event", G_CALLBACK(window_destroy), NULL);
	g_idle_add((GSourceFunc)update_camera, camera_win);
	gtk_widget_show_all(window);
}
