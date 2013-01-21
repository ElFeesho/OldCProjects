#include <gdk/gdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "globals.h"
#include "network.h"
#include "main_window.h"

#ifndef __WIN32__
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

static GdkPixmap *draw_back_buffer = NULL;
static GdkColormap *draw_colormap = NULL;

static int draw_stack[16777216];
static int draw_stack_pos = 0;

static int draw_stack_push(int pos)
{
	if(draw_stack_pos<16777216)
	{
		if(draw_stack_pos<0) 
			draw_stack_pos = 0;
		draw_stack[draw_stack_pos++] = pos;
		return 0;
	}
	else
	{
		return -1;
	}
}

static int draw_stack_pop()
{
	return (draw_stack_pos>=0)?(draw_stack[--draw_stack_pos]):(-1);
}

static void draw_back_buffer_init()
{
	draw_back_buffer = gdk_pixmap_new(NULL,600,600,24);
	draw_colormap = gdk_colormap_new(gdk_visual_get_system(),1);
}

GdkColor *draw_color_create(gint16 red, gint16 green, gint16 blue);
void draw_clear();
void draw_clear_buffers();
void draw_line(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width);
void draw_welcome();
void draw_sync_buffers(GdkEventExpose *ev);

GdkColor *draw_color_create(gint16 red, gint16 green, gint16 blue)
{
	GdkColor *return_color = malloc(sizeof(GdkColor));
	return_color->red = red;
	return_color->green = green;
	return_color->blue = blue;
	return_color->pixel = 0xffffffff;
	return return_color;
}

void draw_welcome()
{
	draw_back_buffer_init();
	draw_clear_buffers();
}

void draw_line(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width)
{
	if(main_window_canvas_win!=NULL)
	{
		GdkGC *gc = gdk_gc_new(main_window_canvas_win);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		GdkColor *col = draw_color_create(r,g,b);
		gdk_gc_set_rgb_fg_color(gc,col);
		gdk_draw_line(main_window_canvas_win,gc,x,y,x2,y2);
		g_object_unref(gc);
		gc = gdk_gc_new(draw_back_buffer);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		gdk_gc_set_rgb_fg_color(gc,col);
		free(col);
		gdk_draw_line(draw_back_buffer,gc,x,y,x2,y2);
		g_object_unref(gc);
	}
}

void draw_rect(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width)
{
	if(main_window_canvas_win!=NULL)
	{
		GdkGC *gc = gdk_gc_new(main_window_canvas_win);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		GdkColor *col = draw_color_create(r, g, b);
		gdk_gc_set_rgb_fg_color(gc,col);
		gdk_draw_rectangle(main_window_canvas_win,gc,0,min(x,x2),min(y,y2),max(x,x2)-min(x,x2),max(y,y2)-min(y,y2));
		g_object_unref(gc);
		gc = gdk_gc_new(draw_back_buffer);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		gdk_gc_set_rgb_fg_color(gc, col);
		free(col);
		gdk_draw_rectangle(draw_back_buffer,gc,0,min(x,x2),min(y,y2),max(x,x2)-min(x,x2),max(y,y2)-min(y,y2));
		g_object_unref(gc);
	}
}

void draw_circle(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width)
{
	if(main_window_canvas_win!=NULL)
	{
		GdkGC *gc = gdk_gc_new(main_window_canvas_win);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		GdkColor *col = draw_color_create(r,g,b);
		gdk_gc_set_rgb_fg_color(gc,col);
		gdk_draw_arc(main_window_canvas_win,gc,0,min(x,x2),min(y,y2),max(x2,x)-min(x2,x),max(y2,y)-min(y2,y),0,64*360);
		g_object_unref(gc);
		
		gc = gdk_gc_new(draw_back_buffer);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		gdk_gc_set_rgb_fg_color(gc,col);
		free(col);
		gdk_draw_arc(draw_back_buffer,gc,0,min(x,x2),min(y,y2),max(x2,x)-min(x2,x),max(y2,y)-min(y2,y),0,64*360);
		g_object_unref(gc);
	}
}

void draw_temp_line(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width)
{
	if(main_window_canvas_win!=NULL)
	{
		GdkGC *gc = gdk_gc_new(main_window_canvas_win);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_function(gc,GDK_INVERT);
		gdk_gc_set_colormap(gc,draw_colormap);
		GdkColor *col = draw_color_create(r,g,b);
		gdk_gc_set_rgb_fg_color(gc, col);;
		gdk_draw_line(main_window_canvas_win,gc,x,y,x2,y2);
		free(col);
		g_object_unref(gc);
	}
}

void draw_temp_outline(int x, int y, int stroke)
{
	if(main_window_canvas_win!=NULL)
	{
		GdkGC *gc = gdk_gc_new(main_window_canvas_win);
		gdk_gc_set_line_attributes(gc,1,GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
		gdk_gc_set_function(gc,GDK_INVERT);
		gdk_gc_set_colormap(gc,draw_colormap);
		GdkColor *col = draw_color_create(0,0,0);
		gdk_gc_set_rgb_fg_color(gc,col);
		free(col);
		gdk_draw_arc(main_window_canvas_win,gc,0,x-stroke/2,y-stroke/2,(stroke>1)?(stroke-1):(stroke),(stroke>1)?(stroke-1):(stroke),0,64*360);
		g_object_unref(gc);
	}
}

void draw_temp_rect(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width)
{
	if(main_window_canvas_win!=NULL)
	{
		GdkGC *gc = gdk_gc_new(main_window_canvas_win);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_function(gc,GDK_INVERT);
		gdk_gc_set_colormap(gc,draw_colormap);
		GdkColor *col = draw_color_create(r,g,b);
		gdk_gc_set_rgb_fg_color(gc,col);
		free(col);
		gdk_draw_rectangle(main_window_canvas_win,gc,0,min(x,x2),min(y,y2),max(x,x2)-min(x,x2),max(y,y2)-min(y,y2));
		g_object_unref(gc);
	}
}

void draw_temp_circle(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width)
{
	if(main_window_canvas_win!=NULL)
	{
		GdkGC *gc = gdk_gc_new(main_window_canvas_win);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_function(gc,GDK_INVERT);
		gdk_gc_set_colormap(gc,draw_colormap);
		GdkColor *col = draw_color_create(r,g,b);
		gdk_gc_set_rgb_fg_color(gc,col);
		free(col);
		gdk_draw_arc(main_window_canvas_win,gc,0,min(x,x2),min(y,y2),max(x2,x)-min(x2,x),max(y2,y)-min(y2,y),0,64*360);
		g_object_unref(gc);
	}
}

void draw_fill_pixels(int x, int y, int w, int h, int bpp, guint8 r, guint8 g, guint8 b, guint8 nr, guint8 ng, guint8 nb, unsigned char *pixels)
{
	if(nr == r && nb == b && ng == g) return; //avoid infinite loop
	draw_stack_pos = 0;
	int pos;
	if(draw_stack_push(x*4+y*w)) return;
	
	while((pos = draw_stack_pop())>0)
	{
		unsigned char * pixels_data = (pixels+pos);
		pixels_data[0] = nb;
		pixels_data[1] = ng; 
		pixels_data[2] = nr;
		if((pos+4)-(pos/w)*w < w && (pixels[pos+4] == b && pixels[pos+5] == g && pixels[pos+6] == r))
		{          
			if(draw_stack_push(pos+4)) return;           
		}    
		if((pos-4)-(pos/w)*w >= 1 && (pixels[pos-4] == b && pixels[pos-3] == g && pixels[pos-2] == r))
		{
			if(draw_stack_push(pos-4)) return;           
		}    
		if(((pos/w)+1) + 1 < h && (pixels[pos+w] == b && pixels[pos+w+1] == g && pixels[pos+w+2] == r))
		{
			if(draw_stack_push(pos+w)) return;           
		}    
		if(((pos/w)+1) - 1 >= 1 && (pixels[pos-w] == b && pixels[pos-w+1] == g && pixels[pos-w+2] == r))
		{
			if(draw_stack_push(pos-w)) return;           
		}    
	}  
}

void draw_flood_fill(int x, int y, guint8 r, guint8 g, guint8 b)
{
	GdkImage *backbuff = gdk_drawable_get_image(draw_back_buffer,0,0,-1,-1);
	int w, h, bpp;
	w = backbuff->width;
	h = backbuff->height;
	bpp = backbuff->bpp;
	unsigned char *memory = (unsigned char*)backbuff->mem;
	draw_fill_pixels(x,y,w*4,h,bpp,memory[(x*bpp+y*(w*4))+2],memory[(x*bpp+y*(w*4))+1],memory[(x*bpp+y*(w*4))],r,g,b,memory);
	GdkGC *gc = gdk_gc_new(draw_back_buffer);
	gdk_draw_image(draw_back_buffer,gc,backbuff,0,0,0,0,-1,-1);
	g_object_unref(gc);
	g_object_unref(backbuff);
	draw_sync_buffers(NULL);
}

void draw_clear_buffers()
{
	GdkGC *gc = gdk_gc_new(main_window_canvas_win);
	GdkColor *col = draw_color_create(65535,65535,65535);
	gdk_gc_set_foreground(gc,col);
	int w, h;
	gdk_drawable_get_size(main_window_canvas_win,&w,&h);
	gdk_draw_rectangle(main_window_canvas_win, gc,1,0,0,w,h);
	g_object_unref(gc);
	
	gc = gdk_gc_new(draw_back_buffer);
	gdk_gc_set_colormap(gc,draw_colormap);
	gdk_drawable_get_size(draw_back_buffer,&w,&h);
	gdk_gc_set_rgb_fg_color(gc,col);
	free(col);
	gdk_draw_rectangle(draw_back_buffer, gc,1,0,0,w,h);
	g_object_unref(gc);
}

void draw_clear()
{
	network_send_clear();
	draw_clear_buffers();
}

void draw_sync_buffers(GdkEventExpose *ev)
{
	GdkGC *gc = gdk_gc_new(main_window_canvas_win);
	if(ev!=NULL)
		gdk_draw_drawable(main_window_canvas_win,gc,draw_back_buffer,ev->area.x,ev->area.y,ev->area.x,ev->area.y,ev->area.width,ev->area.height);
	else
		gdk_draw_drawable(main_window_canvas_win,gc,draw_back_buffer,0,0,0,0,-1,-1);
	g_object_unref(gc);
}

void draw_resize_buffer(int w, int h)
{
	int width, height;
	gdk_drawable_get_size(draw_back_buffer,&width,&height);
	if(w<width && h<height)
		return; /* Don't do anything if we are shrinking! */
	GdkPixmap *n_buffer = gdk_pixmap_new(NULL,w,h,24);
	GdkGC *gc = gdk_gc_new(n_buffer);
	GdkColor *col = draw_color_create(65535,65535,65535);
	gdk_gc_set_foreground(gc,col);
	free(col);
	gdk_drawable_get_size(draw_back_buffer,&width,&height);
	gdk_draw_rectangle(n_buffer, gc,1,0,0,w,h);
	gdk_draw_drawable(n_buffer, gc, draw_back_buffer,0,0,0,0,width,height);
	g_object_unref(draw_back_buffer);
	g_object_unref(gc);
	draw_back_buffer = n_buffer;
}

unsigned char draw_16_to_8(guint16 value)
{
	unsigned char return_val;
	return_val = (unsigned char)((double)((double)(value)/65535.0)*255.0);
	return return_val;
}

GdkPixmap *draw_get_backbuffer()
{
	return draw_back_buffer;
}
