#include <gtk/gtk.h>
#include <stdlib.h>

const char DRAW_LINE = 	1;  // These are used in the draw_list in order to 
const char DRAW_RECT = 	2;	// determine how to use the values stored in the 
const char DRAW_CIRC = 	3;	// list. So if it's type was DRAW_LINE then x,y
const char DRAW_TRI = 	4;	// x2 and y2 would be used instead of just x and y if it was a circle!

typedef struct _draw_list
{
	int x,y,x2,y2,r,g,b,width,radius,begin,end,scalex,scaley,rotation,type,selected;
	void *next;
}draw_list;

static draw_list *draw_list_ins = NULL;
static GdkColor *draw_color_create(gint16 red, gint16 green, gint16 blue);
static GdkPixmap *back_buffer = NULL;
static GdkColormap *draw_colormap = NULL;
static void draw_init_buffer();

void draw_init_canvas(GtkWidget *canvas);
void draw_line(GtkWidget *canvas, int x, int y, int x2, int y2, int r, int g, int b, int width);
void draw_fill_buffer();
void draw_sync_buffers(GtkWidget *canvas);

GdkColor *draw_color_create(gint16 red, gint16 green, gint16 blue)
{
	GdkColor *return_color = malloc(sizeof(GdkColor));
	return_color->red = red;
	return_color->green = green;
	return_color->blue = blue;
	return_color->pixel = 0xffffffff;
	return return_color;
}

void draw_init_buffer()
{
	back_buffer = gdk_pixmap_new(NULL,768,576,24);
	draw_colormap = gdk_colormap_new(gdk_visual_get_system(),1);
}

void draw_init_canvas(GtkWidget *canvas)
{
	if(canvas == NULL)
	{
		printf("Failed to initialise Canvas!\n");
		return;
	}
	
	GdkColor *new_col = malloc(sizeof(GdkColor));
	new_col->red = 65535;
	new_col->blue = 65535;
	new_col->green = 65535;
	
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
	gdk_gc_set_rgb_fg_color(gc,draw_color_create(0,0,0));
	gdk_draw_rectangle(back_buffer, gc,0,0,0,767,575);
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
	cairo_t *cr = gdk_cairo_create(canvas->window);
	cairo_set_line_width(cr, width);
	cairo_set_source_rgb(cr,(double)(r/255),(double)(g/255),(double)(b/255));
	cairo_move_to(cr,x,y);
	cairo_line_to(cr,x2,y2);
	cairo_stroke(cr);
	cairo_destroy(cr);
}

void draw_circle(GtkWidget *canvas, int x, int y, int radius, int r, int g, int b, int width)
{
	cairo_t *cr = gdk_cairo_create(canvas->window);
	cairo_set_line_width(cr, width);
	cairo_set_source_rgb(cr,(double)(r/255),(double)(g/255),(double)(b/255));
	cairo_arc(cr, x,y, radius,0,360 * (3.146/180));
	cairo_stroke(cr);
	cairo_destroy(cr);
	
	if(!back_buffer)
		draw_init_buffer();
		
	cr = gdk_cairo_create(back_buffer);
	cairo_set_line_width(cr, width);
	cairo_set_source_rgb(cr,(double)(r/255),(double)(g/255),(double)(b/255));
	cairo_arc(cr, x,y, radius,0,360 * (3.146/180));
	cairo_stroke(cr);
	cairo_destroy(cr);
}

void draw_temp_circle(GtkWidget *canvas, int x, int y, int radius, int r, int g, int b, int width)
{
	cairo_t *cr = gdk_cairo_create(canvas->window);
	cairo_set_line_width(cr, width);
	cairo_set_source_rgb(cr,(double)(r/255),(double)(g/255),(double)(b/255));
	cairo_arc(cr, x,y, radius,0,360 * (3.146/180));
	cairo_stroke(cr);
	cairo_destroy(cr);
}

void draw_border(GtkWidget *canvas)
{
	GdkGC *gc = gdk_gc_new(canvas->window);
	gdk_gc_set_rgb_fg_color(gc,draw_color_create(0,0,0));
	gdk_draw_rectangle(back_buffer, gc,0,0,0,767,575);
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

void draw_sync_buffers(GtkWidget *canvas)
{
	if(back_buffer==NULL)
		draw_fill_buffer(); //Best to draw nothing than crash out
	gdk_draw_drawable(canvas->window,gdk_gc_new(canvas->window),back_buffer,0,0,0,0,768,576);
}

void draw_add_line(int x, int y, int x2, int y2, int r, int g, int b, int width)
{
	if(draw_list_ins==NULL)
	{
		draw_list_ins = malloc(sizeof(draw_list));
		draw_list_ins->x = x;
		draw_list_ins->y = y;
		draw_list_ins->x2 = x2;
		draw_list_ins->y2 = y2;
		draw_list_ins->width = width;
		draw_list_ins->type = DRAW_LINE;
		draw_list_ins->r = r;
		draw_list_ins->g = g;
		draw_list_ins->b = b;
		draw_list_ins->radius = 0;
		draw_list_ins->begin = 0;
		draw_list_ins->end = 0;
		draw_list_ins->selected = 0;
		draw_list_ins->next = NULL;
	}
	else
	{
		draw_list *temp = malloc(sizeof(draw_list));
		temp->type = DRAW_LINE;
		temp->width = width;
		temp->x = x;
		temp->x2 = x2;
		temp->y = y;
		temp->y2 = y2;
		temp->r = r;
		temp->g = g;
		temp->b = b;
		temp->radius = 0;
		temp->begin = 0;
		temp->end = 0;
		temp->selected = 0;
		temp->next = draw_list_ins;
		draw_list_ins = temp;
	}
}
