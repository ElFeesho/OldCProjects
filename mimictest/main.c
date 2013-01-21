#include <stdio.h>
#include <gtk/gtk.h>
#include <gnet.h>
#include <stdio.h>
#include <stdint.h>
#include <mimic.h>
static int initd = 0;
MimCtx *ctx;
GtkWidget *canvas;
gboolean network_receive(GIOChannel *source, GIOCondition condition, gpointer data)
{
	if((condition&G_IO_HUP)==G_IO_HUP)
	{
		printf("Connection closed\n");
		return FALSE;
	}
	else if((condition&G_IO_ERR)==G_IO_ERR)
	{
		printf("Network error\n");
		return FALSE;
	}
	else if((condition&G_IO_IN) == G_IO_IN)
	{
		GIOError error = G_IO_ERROR_NONE;
		int32_t header;
		gsize n = 4;
		error = gnet_io_channel_readn(source, &header, n, &n); 
		if(error!=G_IO_ERROR_NONE)
		{
			printf("Failed receiving data\n");
			return FALSE;
		}
		n = header;
		char image_data[header];
		error = error = gnet_io_channel_readn(source, image_data, n, &n); 
		if(error!=G_IO_ERROR_NONE)
		{
			printf("Failed getting image data\n");
		}
		printf("Getting image data\n");
		if(!initd)
		{
			mimic_decoder_init(ctx, image_data);
			initd = 1;
		}
		guchar out_put[320*240*3] = { 0 };
		if(mimic_decode_frame(ctx,image_data,out_put))
			gdk_draw_rgb_image(canvas->window,gdk_gc_new(canvas->window),0,0,320,240,GDK_RGB_DITHER_NONE,out_put,320*3);
		else
		{
			printf("Returned false...\nBut what the hey :D\n");
			gdk_draw_rgb_image(canvas->window,gdk_gc_new(canvas->window),0,0,320,240,GDK_RGB_DITHER_NONE,out_put,320*3);
		}
	}
	return TRUE;
}


void got_con(GServer *server, GConn *con, gpointer user_data)
{
	printf("Got connection\n");
	GIOChannel *client_chan = gnet_tcp_socket_get_io_channel(con->socket);
	g_io_channel_set_buffer_size(client_chan,128);
	g_io_channel_set_buffered(client_chan,1);
	g_io_channel_set_encoding(client_chan, NULL, NULL);
	g_io_add_watch(client_chan,G_IO_IN|G_IO_HUP|G_IO_ERR,(GIOFunc)network_receive,NULL);
}

gboolean expose_canvas(GtkWidget *canv, GdkEventExpose *ev, gpointer user_data)
{
	FILE *image = fopen("test.ppm","r");
	char image_data[320*240*3] = { 0 };

	fread(image_data,1,320*240*3,image);
//	gdk_draw_rgb_image(canv->window,gdk_gc_new(canv->window),0,0,320,240,GDK_RGB_DITHER_NONE,image_data,320*3);

	fclose(image);
	return TRUE;
}

int main(int argc, char **argv)
{
	ctx = mimic_open();
	gnet_init();
	GServer *mimmic_server = gnet_server_new(NULL,3456, (GServerFunc)got_con,NULL);
	gtk_init(&argc, &argv);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	canvas = gtk_drawing_area_new();
	gtk_widget_set_size_request(canvas,320,240);
	gtk_container_add(GTK_CONTAINER(window),canvas);	

	g_signal_connect(G_OBJECT(canvas),"expose-event",G_CALLBACK(expose_canvas),NULL);
	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}
