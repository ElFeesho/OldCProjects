#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "draw.h"

/* Callback Declarations */

static void colour_widget_click(GtkWidget *colour, GdkEventButton *ev, gpointer colour_data);

/* End Callback Declarations */

static GdkColor *colour_widget_get_colour(int x, int y);

static GtkWidget *colour_widget_create()
{
	GtkWidget *table;
	
	table = gtk_table_new(8,7,1);
	gtk_widget_set_size_request(table,160,160);
	int i;
	short r = 0,g = 0,b = 0;
	for(i = 0; i<56;i++)
	{
		if(i<8)
		{
			r=65535*(i+1)/8;
			g = 0;
			b = 0;
		}
		
		if(i>=8 && i<16)
		{
			g=65535*(i+1)/8;
			r = 0;
			b = 0;
		}
		
		if(i>=16 && i<24)
		{
			b=65535*(i+1)/8;
			r = 0;
			g = 0;
		}
		
		if(i>=24 && i<32)
		{
			r = 65535*(i+1)/8;
			g = 65535*(i+1)/8;
			b = 0;
		}
		
		if(i>=32 && i<40)
		{
			b = 65535*(i+1)/8;
			g = 65535*(i+1)/8;
			r = 0;
		}
		
		if(i>=40 && i<48)
		{
			r = 65535*(i+1)/8;
			b = 65535*(i+1)/8;
			g = 0;
		}
		if(i>=48 && i<56)
		{
			r = 65535*(i+1)/8;
			g = 65535*(i+1)/8;
			b = 65535*(i+1)/8;;
		}
		
		GtkWidget *colour_block = gtk_event_box_new();
		GdkColor *colour_col = draw_color_create(r,g,b);
		gtk_widget_modify_bg(colour_block,GTK_STATE_NORMAL,colour_col);
		gtk_widget_add_events(colour_block,GDK_BUTTON_PRESS_MASK);
		g_signal_connect(G_OBJECT(colour_block),"button-press-event",G_CALLBACK(colour_widget_click),NULL);
		gtk_table_attach_defaults(GTK_TABLE(table),colour_block,i-(i/8)*8,i-(i/8)*8+1,i/8,i/8+1);
	}
	return table;
}

GtkWidget *colour_widget_get_widget()
{
	return colour_widget_create();
}

/* Callback Declarations */

void colour_widget_click(GtkWidget *colour, GdkEventButton *ev, gpointer colour_data)
{
	GdkColor *col = colour_widget_get_colour(ev->x_root,ev->y_root);
	if(col)
	{
		globals_red = col->red;
		globals_blue = col->blue;
		globals_green = col->green;
	}
	else
	{
		printf("The plot thickens...\n");
		globals_red = 0;
		globals_blue = 0;
		globals_green = 0;
	}
}

/* End Callback Declarations */

GdkColor *colour_widget_get_colour(int xroot, int yroot)
{
	GdkScreen *screen = gdk_screen_get_default();
	GdkImage *image;
	guint32 pixel;
	GdkColor *color = malloc(sizeof(GdkColor));
	GdkColormap *colormap = gdk_screen_get_system_colormap (screen);
	GdkWindow *root_window = gdk_screen_get_root_window (screen);
	
	image = gdk_drawable_get_image (root_window, xroot, yroot, 1, 1);
	if (!image)
	{
		gint x, y;
		GdkDisplay *display = gdk_screen_get_display (screen);
		GdkWindow *window = gdk_display_get_window_at_pointer (display, &x, &y);
		if (!window)
			return 0;
		image = gdk_drawable_get_image (window, x, y, 1, 1);
		if (!image)
			return 0;
	}
	pixel = gdk_image_get_pixel (image, 0, 0);
	g_object_unref (image);

	gdk_colormap_query_color (colormap, pixel, color);
	return color;
}
