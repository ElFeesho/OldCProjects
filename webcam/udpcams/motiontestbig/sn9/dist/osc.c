/* This code is (c) Ian Molton. It is GPL, however I *must be asked for permission to use it* should you desire to use it in your own project. */

#include <gtk/gtk.h>
#include <linux/soundcard.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "sonixbits.c"

#define WIDTH 640
#define HEIGHT 480

struct usb_dev_handle *dev;

/* Backing pixmap for drawing area */
static GdkPixbuf *pixbuf = NULL;
float *buffer = NULL;

/* Create a new backing pixmap of the appropriate size */
static gint configure_event( GtkWidget         *widget,
                             GdkEventConfigure *event )
{
  if (pixbuf)
    g_object_unref (pixbuf);

  pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, 0, 8, WIDTH, HEIGHT);

  return TRUE;
}

/* Redraw the screen from the backing pixmap */
static gint expose_event( GtkWidget      *widget,
                          GdkEventExpose *event )
{
  gdk_pixbuf_render_to_drawable (pixbuf, widget->window,
		     widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		     0, 0, 0, 0, WIDTH, HEIGHT, GDK_RGB_DITHER_NONE, 0, 0);

  return FALSE;
}

int find_header (unsigned char *buffer, int length){
	int i;
	
	i = 0;

	while(i < length-12){
		if(buffer[i] == 0xff && buffer[i+1] == 0xff && buffer[i+2] == 0x00 && buffer[i+3] == 0xc4 && buffer[i+4] == 0xc4 && buffer[i+5] == 0x96 && buffer[i+6] == 0x00)
			return i;
		i++;
	}
	return -1;
}


/* Draw the graph */

int get_bit(unsigned char *data, int *position, int reset){
        static int bit;
        static unsigned char byte;

        if(reset){
                bit = 0;
                byte = data[*position]; *position++;
        }

        if(bit < 7){
                bit++;
                return (byte >> (7-bit)) & 1;
        }

        bit = 1;
        byte = data[*position]; *position++;

        return byte >> 7;
}

int get_byte(unsigned char last, unsigned char *data, int *position, int reset){
        int i;
        int b;

        b = get_bit(data, position, reset);

        if(b == 0)
                return last;

        i = get_bit(data, position, 0) << 1 | get_bit(data, position, 0);

        switch(i){
                case 1:
                return last+3;
                case 2:
                return last-3;
                case 0:
                        i = get_bit(data, position, 0);
                        if(i)
                                return last-8;
                        return last+8;
                case 3:
                        i = get_bit(data, position, 0);
                        if(i)
                                return last-18;
                        i = get_bit(data, position, 0);
                        if(!i)
                                return last+18;
                        b = 0;
                        for(i = 4 ; i >=0 ; i--)
                                b |= get_bit(data, position, 0) << i;
                        return (8*b)+4;
        }
        return last;
}

static gboolean draw_graph(gpointer data){
	GtkWidget *widget = data;
	unsigned char *pixdata = gdk_pixbuf_get_pixels(pixbuf);
	static unsigned char databuffer[4*1024*1024];
	int i, o, x, start, end;

	usb_bulk_read(dev, 0x82, databuffer, 352*288*3, 1000);	

	start = find_header(databuffer, 352*288*3); printf("start: %d\n", start);
	if(start == -1)
		return TRUE;
	end = find_header(databuffer+start+12, 352*288*3 - start - 12);
	if(end == -1)
		end = 352*288*3;
	else
		end += start+12;
	printf("len: %d\n", end-start);

	if(end-start < 128){
		for(i = 0 ; i < end-start ; i++)
			printf("<%02x> ", databuffer[start+i]);fflush(0);
		printf("\n");
	}

	i = start+12; o = 0;
	while(i < end && o < WIDTH*HEIGHT*3 && i < end){
		pixdata[o] = databuffer[i]; o++ ; i++;
		pixdata[o] = databuffer[i]; o++ ; i++;
		o++;
		for(x = 0 ; x < WIDTH-2 && o < WIDTH*HEIGHT*3 && i < end; x++){
			pixdata[o] = get_byte(pixdata[o-3], databuffer, &i, x?0:1);
			o+=3;
		}
		o+=3;
	}
	
  gtk_widget_queue_draw_area (widget, 0,0,widget->allocation.width, widget->allocation.height);
	return TRUE;
}

void quit ()
{
  exit (0);
}

int main( int   argc, 
          char *argv[] )
{
  GtkWidget *window;
  GtkWidget *drawing_area;
  GtkWidget *vbox;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (window, "Test Input");

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (vbox);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (quit), NULL);

  /* Create the drawing area */

  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (GTK_WIDGET (drawing_area), WIDTH, HEIGHT);
  gtk_box_pack_start (GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);

  gtk_widget_show (drawing_area);

  /* Signals used to handle backing pixmap */

  g_signal_connect (G_OBJECT (drawing_area), "expose_event",
		    G_CALLBACK (expose_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area),"configure_event",
		    G_CALLBACK (configure_event), NULL);

  gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
			 | GDK_LEAVE_NOTIFY_MASK);

  dev = get_dev();
  gtk_widget_show (window);


	gtk_timeout_add(10, draw_graph, (gpointer)drawing_area);

  gtk_main ();

  return 0;
}
