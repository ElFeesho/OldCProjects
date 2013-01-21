#include <gdk/gdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "globals.h"
#include "network.h"
#include "main_window.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

static GdkPixmap *draw_back_buffer = NULL;
static GdkColormap *draw_colormap = NULL;

static int draw_stack[16777216];
static int draw_stack_pos = 0;

static int draw_stack_push(int pos)
{
	if(draw_stack_pos<16777216)
	{
		if(draw_stack_pos<0) draw_stack_pos = 0;
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
void draw_sync_buffers();

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
		gdk_gc_set_rgb_fg_color(gc,draw_color_create(r,g,b));
		gdk_draw_line(main_window_canvas_win,gc,x,y,x2,y2);
		
		gc = gdk_gc_new(draw_back_buffer);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		gdk_gc_set_rgb_fg_color(gc,draw_color_create(r,g,b));
		gdk_draw_line(draw_back_buffer,gc,x,y,x2,y2);
	}
}

void draw_rect(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width)
{
	if(main_window_canvas_win!=NULL)
	{
		GdkGC *gc = gdk_gc_new(main_window_canvas_win);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		gdk_gc_set_rgb_fg_color(gc,draw_color_create(r,g,b));
		gdk_draw_rectangle(main_window_canvas_win,gc,0,min(x,x2),min(y,y2),max(x,x2)-min(x,x2),max(y,y2)-min(y,y2));
		
		gc = gdk_gc_new(draw_back_buffer);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		gdk_gc_set_rgb_fg_color(gc,draw_color_create(r,g,b));
		gdk_draw_rectangle(draw_back_buffer,gc,0,min(x,x2),min(y,y2),max(x,x2)-min(x,x2),max(y,y2)-min(y,y2));
	}
}

void draw_temp_line(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width)
{
	if(main_window_canvas_win!=NULL)
	{
		GdkGC *gc = gdk_gc_new(main_window_canvas_win);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		gdk_gc_set_rgb_fg_color(gc,draw_color_create(r,g,b));
		gdk_draw_line(main_window_canvas_win,gc,x,y,x2,y2);
	}
}

void draw_temp_outline(int x, int y, int stroke)
{
	if(main_window_canvas_win!=NULL)
	{
		GdkGC *gc = gdk_gc_new(main_window_canvas_win);
		gdk_gc_set_line_attributes(gc,1,GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		gdk_gc_set_rgb_fg_color(gc,draw_color_create(0,0,0));
		gdk_draw_arc(main_window_canvas_win,gc,0,x-stroke/2,y-stroke/2,(stroke>1)?(stroke-1):(stroke),(stroke>1)?(stroke-1):(stroke),0,64*360);
	}
}

void draw_temp_rect(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width)
{
	if(main_window_canvas_win!=NULL)
	{
		GdkGC *gc = gdk_gc_new(main_window_canvas_win);
		gdk_gc_set_line_attributes(gc,width, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_gc_set_colormap(gc,draw_colormap);
		gdk_gc_set_rgb_fg_color(gc,draw_color_create(r,g,b));
		gdk_draw_rectangle(main_window_canvas_win,gc,0,min(x,x2),min(y,y2),max(x,x2)-min(x,x2),max(y,y2)-min(y,y2));
	}
}


#define MAX 10000

typedef struct {
	int y, xl, xr, dy;
} linesegment_t;

void floodFill(int *pixels, int width, int height, int x, int y, int nv)
{
 linesegment_t stack[MAX];
 int l, x1, x2, dy;
 int offset, ov;
 int sp = 0;
 
 // read pv at seed point
 if((ov = pixels[(y * width) + x]) == nv)
  return;
 
 // needed in some cases
 stack[sp].y  = y;
 stack[sp].xl = x;
 stack[sp].xr = x;
 stack[sp].dy = 1;
 sp++;
 
 // seed segment (popped 1st)
 stack[sp].y  = y;
 stack[sp].xl = x;
 stack[sp].xr = x;
 stack[sp].dy = -1;
 sp++;
 
 while(sp > 0)
 {
  // pop segment off stack and fill a neighbouring scan line
  sp--;
  y  = stack[sp].y + (dy = stack[sp].dy);
  x1 = stack[sp].xl;
  x2 = stack[sp].xr;
  
  // segment of scan line y - dy for x1 <= x <= x2 was previously filled,
  //   now explore adjacent pixels in scan line y
  offset = y * width;
  for(x = x1; x >= 0 && pixels[offset + x] == ov; x--)
   pixels[offset + x] = nv;
  
  if(x >= x1) goto skip;
  
  l = x + 1;
  if(l < x1){
   // leak on left?
   if(sp < MAX && y + -dy >= 0 && y + -dy < height){
    stack[sp].y  = y;
    stack[sp].xl = l;
    stack[sp].xr = x1 - 1;
    stack[sp].dy = -dy;
    sp++;
   }
  }
  x = x1 + 1;
  do{
   offset = y * width;
   while(x < width && pixels[offset + x] == ov){
    pixels[offset + x] = nv;
    x++;
   }
   if(sp < MAX && y + dy >= 0 && y + dy < height){
    stack[sp].y  = y;
    stack[sp].xl = l;
    stack[sp].xr = x - 1;
    stack[sp].dy = dy;
    sp++;
   }
   if(x > x2 + 1){
    // leak on right?
    if(sp < MAX && y + -dy >= 0 && y + -dy < height){
     stack[sp].y  = y;
     stack[sp].xl = x2 + 1;
     stack[sp].xr = x - 1;
     stack[sp].dy = -dy;
     sp++;
    }
   }
   
  skip:
   offset = y * width;
   for(x++; x <= x2 && pixels[offset + x] != ov; x++);
   l = x;
   
  }while(x <= x2);
 }
 
 return;
}


void draw_fill_pixels2(int x, int y, int w, int h, int bpp, guint8 r, guint8 g, guint8 b, guint8 nr, guint8 ng, guint8 nb, unsigned char *pixels)
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

void draw_fill_pixels(int x, int y, int w, int h, int bpp, guint8 r, guint8 g, guint8 b, guint8 nr, guint8 ng, guint8 nb, unsigned char *pixels)
{
}

void draw_flood_fill(int x, int y, guint8 r, guint8 g, guint8 b)
{
	GdkImage *backbuff = gdk_drawable_get_image(draw_back_buffer,0,0,600,600);
	int w, h, bpp;
	w = backbuff->width;
	h = backbuff->height;
	bpp = backbuff->bpp;
	unsigned char *memory = (unsigned char*)backbuff->mem;
//	draw_fill_pixels(x,y,w*4,h,bpp,memory[(x*bpp+y*(w*4))+2],memory[(x*bpp+y*(w*4))+1],memory[(x*bpp+y*(w*4))],r,g,b,memory);
	floodFill((unsigned int*)memory, w, h, x, y, ((int)r<<16)|((int)g<<8)|b);
	
	gdk_draw_image(draw_back_buffer,gdk_gc_new(draw_back_buffer),backbuff,0,0,0,0,600,600);
	draw_sync_buffers();
}

void draw_clear_buffers()
{
	GdkGC *gc = gdk_gc_new(main_window_canvas_win);
	gdk_gc_set_foreground(gc,draw_color_create(65535,65535,65535));
	gdk_draw_rectangle(main_window_canvas_win, gc,1,0,0,600,600);
	
	gc = gdk_gc_new(draw_back_buffer);
	gdk_gc_set_colormap(gc,draw_colormap);
	gdk_gc_set_rgb_fg_color(gc,draw_color_create(65535,65535,65535));
	gdk_draw_rectangle(draw_back_buffer, gc,1,0,0,600,600);
}

void draw_clear()
{
	network_send_clear();
	draw_clear_buffers();
}

void draw_sync_buffers()
{
	gdk_draw_drawable(main_window_canvas_win,gdk_gc_new(main_window_canvas_win),draw_back_buffer,0,0,0,0,600,600);
}

unsigned char draw_16_to_8(guint16 value)
{
	unsigned char return_val;
	return_val = (unsigned char)((double)((double)(value)/65535.0)*255.0);
	return return_val;
}
