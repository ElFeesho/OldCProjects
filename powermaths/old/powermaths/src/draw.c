#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <librsvg/rsvg.h>
#include "question.h"

static GdkColor *draw_color_create(gint16 red, gint16 green, gint16 blue);
static GdkPixmap *back_buffer = NULL;
static GdkPixmap *original_question = NULL;
static GdkColormap *draw_colormap = NULL;
static void draw_init_buffer();
static void draw_border(GtkWidget *canvas);

void draw_init_canvas(GtkWidget *canvas);
void draw_line(GtkWidget *canvas, int x, int y, int x2, int y2, int r, int g, int b, int width);
void draw_fill_buffer();
void draw_sync_buffers(GtkWidget *canvas);

void draw_question_number(GtkWidget *widget, int x, int y, int number)
{
	cairo_t *cr = gdk_cairo_create(widget->window);
	
	cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, 25);
	cairo_move_to (cr, x+10,y+18);
	char text[4] = { 0 };
	sprintf(text,"%d.",number);
	cairo_show_text (cr, text);
	cairo_fill(cr);

	cairo_move_to(cr,x,y);
	cairo_rel_curve_to(cr,0,-10,0,-10,10,-10);
	cairo_rel_line_to(cr,20,0);
	cairo_rel_curve_to(cr,10,0,10,0,10,10);
	cairo_rel_line_to(cr,0,10);
	cairo_rel_curve_to(cr,0,20,0,20,-10,20);
	cairo_rel_line_to(cr,-20,0);
	cairo_rel_curve_to(cr,-10,0,-10,0,-10,-20);
	cairo_rel_line_to(cr,0,-10);
	
	cairo_pattern_t* gradient = cairo_pattern_create_linear(0,y-5,0,y+25);
	cairo_pattern_add_color_stop_rgba(gradient, 0.0,	0.4,0.4,0.4,0.5);
	cairo_pattern_add_color_stop_rgba(gradient, 0.2,	0.8,0.8,0.8,0.5);
	cairo_pattern_add_color_stop_rgba(gradient, 0.6,	0.4,0.4,0.4,0.5);
	cairo_pattern_add_color_stop_rgba(gradient, 1.0,	0.1,0.1,0.1,0.5);
	cairo_set_source(cr,gradient);
	cairo_fill(cr);

	cairo_destroy(cr);	
}

void draw_thumbnail_question(GtkWidget *widget, struct Question *question_data)
{
	GdkPixbuf *image = NULL;
	if(image==NULL)
	{
		char *filename = malloc(140);
		strcpy(filename,"questions/");
		strcat(filename,question_data->filename);
		image = rsvg_pixbuf_from_file_at_size(filename,widget->allocation.width, widget->allocation.height,NULL);
		free(filename);
	}
	if(image)
		gdk_draw_pixbuf(widget->window, gdk_gc_new(widget->window),image,0,0,0,0,widget->allocation.width, widget->allocation.height,GDK_RGB_DITHER_NONE,0,0);
	else
	{
		cairo_t *cr = gdk_cairo_create(widget->window);
		cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size (cr, 12);
		cairo_move_to(cr,25,60);
		cairo_show_text(cr,"Question Not Found!");
		cairo_fill(cr);
	}
	draw_question_number(widget,5,15,question_data->number);
	draw_border(widget);
}

GdkColor *draw_color_create(gint16 red, gint16 green, gint16 blue)
{
	GdkColor *return_color = malloc(sizeof(GdkColor));
	return_color->red = red;
	return_color->green = green;
	return_color->blue = blue;
	return_color->pixel = 0xffffffff;
	return return_color;
}

GdkColor *draw_color_create_alpha(gint16 red, gint16 green, gint16 blue)
{
	GdkColor *return_color = malloc(sizeof(GdkColor));
	return_color->red = red;
	return_color->green = green;
	return_color->blue = blue;
	return_color->pixel = 0x00000000;
	return return_color;
}

void draw_set_original(GdkPixmap *orig)
{
	original_question = gdk_pixmap_new(NULL,768,576,24);
	if(!draw_colormap)
		draw_colormap = gdk_colormap_new(gdk_visual_get_system(),1);
	GdkGC *gc = gdk_gc_new(original_question);
	gdk_gc_set_colormap(gc,draw_colormap);
	gdk_gc_set_rgb_fg_color(gc,draw_color_create(65535,65535,65535));
	gdk_draw_rectangle(original_question,gc,1,0,0,768,576);
	if(orig)
		gdk_draw_drawable(original_question,gdk_gc_new(orig),orig,0,0,0,0,768,576);
	else
	{
		gdk_gc_set_foreground(gc,draw_color_create(0,0,0));
		gdk_draw_line(original_question,gc,0,0,768,576);
		gdk_draw_line(original_question,gc,768,0,0,576);
	}
	if(!back_buffer)
		draw_init_buffer();
	gdk_draw_drawable(back_buffer,gdk_gc_new(back_buffer),original_question,0,0,0,0,768,576);
}

void draw_init_buffer()
{
	back_buffer = gdk_pixmap_new(NULL,768,576,24);
	if(!draw_colormap)
		draw_colormap = gdk_colormap_new(gdk_visual_get_system(),1);
}

void draw_init_canvas(GtkWidget *canvas)
{
	if(canvas == NULL)
	{
		printf("Failed to initialise Canvas!\n");
		return;
	}
	
	GdkColor *new_col = draw_color_create(65535, 65535, 65535);
	
	gtk_widget_modify_bg(canvas, GTK_STATE_NORMAL, new_col);
	gtk_widget_modify_bg(canvas, GTK_STATE_ACTIVE, new_col);
	gtk_widget_modify_bg(canvas, GTK_STATE_PRELIGHT, new_col);
	gtk_widget_modify_bg(canvas, GTK_STATE_INSENSITIVE, new_col);
	gtk_widget_modify_bg(canvas, GTK_STATE_SELECTED, new_col);
}

/* Should be called after a clear and for buffer initialisation */
void draw_fill_buffer()
{
	if(back_buffer == NULL)
		draw_init_buffer();
	
	GdkGC *gc = gdk_gc_new(back_buffer);
	gdk_gc_set_colormap(gc,draw_colormap);
	gdk_gc_set_rgb_fg_color(gc,draw_color_create(65535,65535,65535));
	gdk_draw_rectangle(back_buffer, gc,1,0,0,768,576);
	if(original_question)
		gdk_draw_drawable(back_buffer,gc,original_question,0,0,0,0,768,576);
}

void draw_fill_a_buffer(GdkDrawable *buffer)
{
	GdkGC *gc = gdk_gc_new(buffer);
	if(!draw_colormap)
		draw_colormap = gdk_colormap_new(gdk_visual_get_system(),1);
	gdk_gc_set_colormap(gc,draw_colormap);
	gdk_gc_set_rgb_fg_color(gc,draw_color_create(65535,65535,65535));
	gdk_draw_rectangle(buffer, gc,1,0,0,768,576);
}

void draw_line(GtkWidget *canvas, int x, int y, int x2, int y2, int r, int g, int b, int width)
{
	cairo_t *cr = gdk_cairo_create(canvas->window);
	cairo_set_line_width(cr, width);
	cairo_set_source_rgb(cr,(double)(r/255),(double)(g/255),(double)(b/255));
	cairo_move_to(cr,x,y);
	cairo_line_to(cr,x2,y2);
	cairo_stroke(cr);
	cairo_destroy(cr);
	
	if(!back_buffer)
		draw_init_buffer();
		
	cr = gdk_cairo_create(back_buffer);
	cairo_set_line_width(cr,width);
	cairo_set_source_rgb(cr,(double)(r/255),(double)(g/255),(double)(b/255));
	cairo_move_to(cr,x,y);
	cairo_line_to(cr,x2,y2);
	cairo_stroke(cr);
	cairo_destroy(cr);
}

void draw_temp_line(GtkWidget *canvas, int x, int y, int x2, int y2, int r, int g, int b, int width)
{
	gdk_draw_line(canvas->window,gdk_gc_new(canvas->window),x,y,x2,y2);
}

void draw_erase(GtkWidget *canvas, int x, int y, int x2, int y2, int width)
{
	GdkGC *gc =	gdk_gc_new(canvas->window);
	gdk_gc_set_foreground(gc,draw_color_create_alpha(1,1,1));
	gdk_gc_set_background(gc,draw_color_create_alpha(1,1,1));
	gdk_draw_line(canvas->window, gc, x, y, x2, y2);
	
	if(!back_buffer)
		draw_init_buffer();
	
	gc =	gdk_gc_new(back_buffer);
	gdk_gc_set_foreground(gc,draw_color_create_alpha(1,1,1));
	gdk_gc_set_background(gc,draw_color_create_alpha(1,1,1));
	gdk_draw_line(back_buffer, gc, x, y, x2, y2);
}

void draw_border(GtkWidget *canvas)
{
	gdk_draw_rectangle(canvas->window, gdk_gc_new(canvas->window),0,0,0,canvas->allocation.width-1,canvas->allocation.height-1);
}

void draw_clear_canvas(GtkWidget *canvas)
{
	draw_fill_buffer();
	cairo_t *cr = gdk_cairo_create(canvas->window);
	cairo_set_source_rgb(cr,1,1,1);
	cairo_rectangle(cr,0,0,canvas->allocation.width, canvas->allocation.height);
	cairo_fill(cr);
	cairo_destroy(cr);
}

void draw_sync_buffers_update()
{
	GdkPixmap *temp_buf = gdk_pixmap_new(back_buffer,768,576,24);
	draw_fill_a_buffer(temp_buf);
	gdk_draw_drawable(temp_buf,gdk_gc_new(temp_buf),original_question,0,0,0,0,768,576);
	gdk_draw_drawable(temp_buf,gdk_gc_new(temp_buf),back_buffer,0,0,0,0,768,576);
	draw_fill_a_buffer(back_buffer);
	gdk_draw_drawable(back_buffer,gdk_gc_new(back_buffer),temp_buf,0,0,0,0,768,576);
}

void draw_sync_buffers(GtkWidget *canvas)
{
	if(back_buffer==NULL)
		draw_fill_buffer(); //Best to draw nothing than crash out
	gdk_draw_drawable(canvas->window,gdk_gc_new(canvas->window),back_buffer,0,0,0,0,768,576);
	
}
