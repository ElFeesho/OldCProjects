#include <stdio.h>
#include <gtk/gtk.h>
#include <gnet.h>
#include <stdio.h>
#include <stdint.h>
#include <mimic.h>

static int initd = 0;
MimCtx *ctx;
GtkWidget *canvas;

GUdpSocket *server_socket;

gboolean network_receive(GIOChannel *source, GIOCondition condition, gpointer data)
{
	if((condition&G_IO_IN) == G_IO_IN)
	{
		int32_t header = 0;
		gnet_udp_socket_receive(server_socket,(gchar*)&header,4,NULL);
		printf("Received header: %d bytes to receive\n");		
		char image_data[header];
		gnet_udp_socket_receive(server_socket,(gchar*)image_data,header,NULL);
		if(!initd)
		{
			mimic_decoder_init(ctx, image_data);
			initd = 1;
			printf("Initialised MIMIC decoding\n");
		}
		guchar out_put[320*240*3];
		if(mimic_decode_frame(ctx,image_data,out_put))
		{
			gdk_draw_rgb_image(canvas->window,gdk_gc_new(canvas->window),0,0,320,240,GDK_RGB_DITHER_NONE,out_put,320*3);
		}
		else
		{
			printf("Failed decoding!\n");
		}
	}
	return TRUE;
}

int main(int argc, char **argv)
{
	ctx = mimic_open();

	g_set_application_name("UDP Webcam Server");

	gnet_init();
	
	server_socket = gnet_udp_socket_new_with_port(3456); // Ok ok it's not a server or whatever?? well it is, oh leave me be!
	
	GIOChannel *client_chan = gnet_udp_socket_get_io_channel(server_socket);
	g_io_channel_set_buffer_size(client_chan,128);
	g_io_channel_set_buffered(client_chan,1);
	g_io_channel_set_encoding(client_chan, NULL, NULL);
	g_io_add_watch(client_chan,G_IO_IN|G_IO_HUP|G_IO_ERR,(GIOFunc)network_receive,NULL);
	gtk_init(&argc, &argv);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	canvas = gtk_drawing_area_new();
	gtk_widget_set_size_request(canvas,320,240);
	gtk_container_add(GTK_CONTAINER(window),canvas);	

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_main_quit),NULL);
	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}
