#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "globals.h"
#include "draw.h"

static void colour_convert(double H, double S, double V, double *R, double *G, double *B)
{
	if( V == 0 )
	{ 
		*R = 0;
		*G = 0;
		*B = 0;
	}
	else if( S == 0 )
	{
		*R = V;
		*G = V;
		*B = V;
	} 
	else
	{
		const double hf = H / 60.0;
		const int    i  = (int) floor( hf );
		const double f  = hf - i;
		const double pv  = V * ( 1 - S );
		const double qv  = V * ( 1 - S * f );
		const double tv  = V * ( 1 - S * ( 1 - f ) );
		switch(i)
		{
			case 0:
				*R = V; 
				*G = tv;
				*B = pv;
				break; 
			case 1:
				*R = qv;
				*G = V; 
				*B = pv;
				break;
			case 2:
				*R = pv;
				*G = V; 
				*B = tv;
				break;
			case 3:
				*R = pv;
				*G = qv;
				*B = V;
				break;
			case 4:
				*R = tv;
				*G = pv;
				*B = V;
				break;
			case 5:
				*R = V;
				*G = pv;
				*B = qv;
				break;
			case 6:
				*R = V;
				*G = tv;
				*B = pv;
				break;
			case -1:
				*R = V;
				*G = pv;
				*B = qv;
				break;
			default:
				fprintf(stderr,"i Value error in Pixel conversion, Value is %d\n",i);
				break;
		}
	}
	*R *= 255.0F;
	*G *= 255.0F;
	*B *= 255.0F;
}


static int lx = 0, ly = 0; /* The last click position */

static GtkWidget *colour_ccol, *colour_vslider; /* The Preview Colour box and vertical slider*/

/* Callback Declarations */

static void colour_widget_move(GtkWidget *colour, GdkEventMotion *ev, gpointer colour_data);
static gboolean colour_vslider_change(GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *canvas);
static void colour_widget_click(GtkWidget *colour, GdkEventButton *ev, gpointer colour_data);
static gboolean colour_widget_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer colour_data);
static void colour_widget_slider_change(GtkWidget *slider, gpointer user_data);

/* End Callback Declarations */

static GdkColor *colour_widget_get_colour(GtkWidget *canvas, int x, int y, double val);

static GtkWidget *colour_widget_create()
{
	GtkWidget *hbox;
	GtkWidget *vslider, *vbox;
	GtkWidget *pallette, *value;

	hbox = gtk_hbox_new(0,0);
	vbox = gtk_vbox_new(0,0);
	pallette = gtk_drawing_area_new();
	gtk_widget_set_size_request(pallette,100,100);

	colour_vslider = gtk_vscale_new_with_range(0,100,1);
	gtk_range_set_inverted(GTK_RANGE(colour_vslider),TRUE);
	colour_ccol = gtk_event_box_new();

	gtk_box_pack_start(GTK_BOX(hbox),pallette,1,1,0);
	gtk_box_pack_start(GTK_BOX(hbox),vbox, 1,1, 0);
	gtk_box_pack_start(GTK_BOX(vbox),colour_vslider, 1,1,0);
	gtk_box_pack_start(GTK_BOX(vbox),colour_ccol,1,1,0);

	gtk_widget_set_events(pallette, GDK_BUTTON_PRESS_MASK|GDK_BUTTON_MOTION_MASK);

	g_signal_connect(G_OBJECT(pallette),"button-press-event", G_CALLBACK(colour_widget_click), NULL);
	g_signal_connect(G_OBJECT(pallette),"motion-notify-event", G_CALLBACK(colour_widget_move), NULL);
	g_signal_connect(G_OBJECT(pallette),"expose-event",G_CALLBACK(colour_widget_expose),NULL);

	g_signal_connect(G_OBJECT(colour_vslider), "change-value", G_CALLBACK(colour_vslider_change), pallette);

	return hbox;
}

GtkWidget *colour_widget_get_widget()
{
	return colour_widget_create();
}

/* Callback Declarations */

gboolean colour_vslider_change(GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *canvas)
{
	GdkColor *bg_col = colour_widget_get_colour(canvas,lx, ly, value/100.0);
	gtk_widget_modify_bg(colour_ccol,GTK_STATE_NORMAL,bg_col);
	globals_red = bg_col->red;
	globals_green = bg_col->green;
	globals_blue = bg_col->blue;
	g_free(bg_col);
	return FALSE;
}

void colour_widget_click(GtkWidget *colour, GdkEventButton *ev, gpointer colour_data)
{
	/* 
		Use cleverness to calculate what the colour would be at the given X and Y! 
		PSYCHIARTRY!!! 
	*/
	lx = ev->x;
	ly = ev->y;
	GdkColor *bg_col = colour_widget_get_colour(colour,lx, ly,1.0);
	gtk_widget_modify_bg(colour_ccol,GTK_STATE_NORMAL,bg_col);
	gtk_range_set_value(GTK_RANGE(colour_vslider),100);
	gtk_widget_queue_draw(colour);
	globals_red = bg_col->red;
	globals_green = bg_col->green;
	globals_blue = bg_col->blue;
	g_free(bg_col);
}

void colour_widget_move(GtkWidget *colour, GdkEventMotion *ev, gpointer colour_data)
{
	/* 
		Use cleverness to calculate what the colour would be at the given X and Y! 
		PSYCHIARTRY!!! 
	*/
	if(ev->x<0) ev->x = 0;
	if(ev->y<0) ev->y = 0;
	if(ev->x>colour->allocation.width) ev->x = colour->allocation.width;
	if(ev->y>colour->allocation.height) ev->y = colour->allocation.height;
	lx = ev->x;
	ly = ev->y;
	GdkColor *bg_col = colour_widget_get_colour(colour,lx, ly,1.0);
	gtk_widget_modify_bg(colour_ccol,GTK_STATE_NORMAL,bg_col);
	gtk_range_set_value(GTK_RANGE(colour_vslider),100);
	gtk_widget_queue_draw(colour);
	globals_red = bg_col->red;
	globals_green = bg_col->green;
	globals_blue = bg_col->blue;
	g_free(bg_col);
}

/* End Callback Declarations */

GdkColor *colour_widget_get_colour(GtkWidget *canvas, int xroot, int yroot, double val)
{
	if(val<0.0) val = 0.0;
	GdkColor *ret_val = g_new(GdkColor,1);
	double sat = 1.0;
	double hue = 0.0;

	hue = xroot/(double)canvas->allocation.width*360.0;
	sat -= (1.0/(double)canvas->allocation.height)*yroot;
	double r, g, b;
	colour_convert(hue,sat,val, &r, &g, &b);
	ret_val->red = r*255;
	ret_val->green = g*255;
	ret_val->blue = b*255;
	return ret_val;
}

gboolean colour_widget_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer colour_data)
{
	static GdkPixmap *bg = NULL;
	GdkGC *cwgc = gdk_gc_new(canvas->window);
	if(!bg)
	{
		bg = gdk_pixmap_new(canvas->window, canvas->allocation.width, canvas->allocation.height, 24);
		int i = 0;
		int k = 0;
		double sat = 1.0;
		double val = 1.0;
		double hue = 0.0;
		for(k = 0;k<canvas->allocation.height;k++)
		{
			for (i = 0;i<canvas->allocation.width;i++)
			{
				hue = i/(double)canvas->allocation.width*360.0;
				GdkGC *gc = gdk_gc_new(bg);
				GdkColor col;
				double r, g, b;
				colour_convert(hue,sat,val, &r, &g, &b);
				col.red = r*255;
				col.green = g*255;
				col.blue = b*255;
				gdk_gc_set_rgb_fg_color(gc, &col);
				gdk_draw_line (bg, gc,i,k,i,k);
				g_object_unref(gc);
			}
			sat -= 1.0/(double)canvas->allocation.height;
		}
		gdk_draw_drawable(canvas->window, cwgc, bg,0,0,0,0,-1,-1);
	}
	else
	{
		gdk_draw_drawable(canvas->window, cwgc, bg,0,0,0,0,-1,-1);
	}
	
	gdk_draw_line(canvas->window, cwgc, lx-3,ly, lx-1,ly);
	gdk_draw_line(canvas->window, cwgc, lx+1,ly, lx+3,ly);
	gdk_draw_line(canvas->window, cwgc, lx,ly-3, lx,ly-1);
	gdk_draw_line(canvas->window, cwgc, lx,ly+1, lx,ly+3);
	g_object_unref(cwgc);
	return TRUE;
}
