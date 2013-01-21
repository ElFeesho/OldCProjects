#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include <stdlib.h>

GdkColor *misc_create_color(short red, short green, short blue)
{
	GdkColor *nColor = malloc(sizeof(GdkColor));
	nColor->red = red;
	nColor->blue = blue;
	nColor->green = green;
	return nColor;
}

typedef struct banner_info
{
	short size;
	char *text;
} banner_info;

static gboolean banner_box_expose(GtkWidget *banner, GdkEventExpose *ev, banner_info *data)
{
	cairo_t *cr = gdk_cairo_create(banner->window);
	cairo_rectangle(cr,0,0,banner->allocation.width, banner->allocation.height);
	
	cairo_pattern_t* gradient = cairo_pattern_create_linear(0,0,0, banner->allocation.height);
	cairo_pattern_add_color_stop_rgba(gradient, 0.0,0.0,0.52,0.77,1.0);
	cairo_pattern_add_color_stop_rgba(gradient, 0.2,0.0,0.8,1.0,1.0);
	cairo_pattern_add_color_stop_rgba(gradient, 0.6,0.0,0.52,0.77,1.0);
	cairo_pattern_add_color_stop_rgba(gradient, 1.0,0.0,0.3,0.44,1.0);
	cairo_set_source(cr,gradient);
	cairo_fill(cr);
	cairo_set_source_rgb(cr,1.0,1.0,1.0);
	cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, data->size);
	cairo_text_extents_t exts;
	cairo_text_extents(cr,data->text,&exts);
	cairo_move_to (cr, banner->allocation.width/2-exts.width/2,banner->allocation.height/2+exts.height/3);
	cairo_show_text (cr, data->text);
	cairo_fill(cr);
	
	cairo_destroy(cr);
	return 1;
}

GtkWidget *misc_create_banner(char *text, unsigned short size)
{
	
	GtkWidget *banner_box = NULL;

	banner_box = gtk_drawing_area_new();
	banner_info *new_info = malloc(sizeof(banner_info));
	new_info->size = size;
	new_info->text = text;
	g_signal_connect(G_OBJECT(banner_box),"expose-event",G_CALLBACK(banner_box_expose),new_info);

	return banner_box;
}
