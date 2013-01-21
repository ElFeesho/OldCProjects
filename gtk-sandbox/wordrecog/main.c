/*
 *	Compile with 
 * 	gcc source.c `pkg-config --libs --cflags gtk+-2.0` -o wordrecog 
 */

#include <gtk/gtk.h>
#include <string.h>

static int startx, starty, minx = 0, miny = 0, maxx = 0, maxy = 0;
static GtkWidget *textbox;

static void check_pixmap(GdkPixmap *theMap)
{
	double width = maxx-minx;
	double height = maxy-miny;
	if(height<10)
		return;
	if(width<5)
	{
		printf("1 - Default\n");
		gtk_entry_append_text(GTK_ENTRY(textbox),"1");
		return;
	}
	double boxw = width/3.0;
	double boxh = height/5.0;
	
	GdkImage *segments[15];
	
	char black[] = { 0,0 ,0 ,0 };
	
	segments[0] = gdk_drawable_get_image(theMap,0,0,boxw,boxh);			
	segments[1] = gdk_drawable_get_image(theMap,boxw,0,boxw,boxh);
	segments[2] = gdk_drawable_get_image(theMap,boxw*2,0,boxw,boxh);
	
	segments[3] = gdk_drawable_get_image(theMap,0,boxh,boxw,boxh);
	segments[4] = gdk_drawable_get_image(theMap,boxw,boxh,boxw,boxh);
	segments[5] = gdk_drawable_get_image(theMap,boxw*2,boxh,boxw,boxh);
	
	segments[6] = gdk_drawable_get_image(theMap,0,boxh*2,boxw,boxh);
	segments[7] = gdk_drawable_get_image(theMap,boxw,boxh*2,boxw,boxh);
	segments[8] = gdk_drawable_get_image(theMap,boxw*2,boxh*2,boxw,boxh);
	
	segments[9] = gdk_drawable_get_image(theMap,0,boxh*3,boxw,boxh);
	segments[10] = gdk_drawable_get_image(theMap,boxw,boxh*3,boxw,boxh);
	segments[11] = gdk_drawable_get_image(theMap,boxw*2,boxh*3,boxw,boxh);
	
	segments[12] = gdk_drawable_get_image(theMap,0,boxh*4,boxw,boxh);
	segments[13] = gdk_drawable_get_image(theMap,boxw,boxh*4,boxw,boxh);
	segments[14] = gdk_drawable_get_image(theMap,boxw*2,boxh*4,boxw,boxh);
	
	int i = 0;
	char checked[15] = { '\0' };
	for(i=0;i<=14;i++)
	{
		int j = 0;
		for(j=0;j<segments[i]->height;j++)
		{
			int k = 0;
			for(k = 0;k<segments[i]->width*4;k+=4)
			{
				if(memcmp(segments[i]->mem+(j*segments[i]->width*4+k),black,4)==0)
				{
					checked[i] = 1;
					break;
				}
			}
			if(checked[i])
				break;
		}
	}

	gchar found[1] = { 0 };
	if(checked[1] && checked[2] && checked[3] && checked[5] && checked[8] && checked[10] && checked[12] && checked[13] && checked[14])
	{
		found[0] = '2';
	} 
	if(checked[0] && checked[1] && checked[2] && checked[5] && checked[7] && checked[8]  && checked[11] && checked[12] && checked[13] && checked[14])
	{
		found[0] = '3';
	} 
	if(checked[1] && checked[3] && checked[4] && checked[6] && checked[7] && checked[8] && checked[10] && checked[13])
	{
		found[0] = '4';
	} 
	if(checked[0] && checked[1] && checked[2] && checked[3] && checked[6] && checked[8]  && checked[11] && checked[12] && checked[13] && checked[14])
	{
		found[0] = '5';
	} 
	if(checked[2] && checked[4] && checked[6] && checked[7] && checked[8] && checked[9] && checked[12] && checked[13] && checked[14])
	{
		found[0] = '6';
	} 
	if(checked[0] && checked[1] && checked[2] && checked[7] && checked[12])
	{
		found[0] = '7';
	} 
	if(checked[0] && checked[1] && checked[2] && checked[3] && checked[5] && checked[6] && checked[7] && checked[8] && checked[9] && checked[11] && checked[12] && checked[13] && checked[14])
	{
		found[0] = '8';
	} 
	if(checked[0] && checked[1] && checked[2] && checked[3] && checked[5] && checked[6] && checked[8] && checked[10] && checked[12])
	{
		found[0] = '9';
	}  
	int pos = 0;
	gtk_editable_insert_text(GTK_EDITABLE(textbox),(gchar*)found,1,&pos);
}

static gboolean canvas_mouse_down(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	startx = ev->x;
	starty = ev->y;
	return 1;
}

static gboolean canvas_mouse_move(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	GdkGC *gc = gdk_gc_new(canvas->window);
	gdk_gc_set_line_attributes(gc,3,GDK_LINE_SOLID,GDK_CAP_ROUND,GDK_JOIN_ROUND);
	gdk_draw_line(canvas->window,gc,startx,starty,ev->x,ev->y);
	
	if(minx == 0 && maxx == 0 && miny == 0 && maxy == 0)
	{
		if(ev->x < startx)
		{
			minx = ev->x;
			maxx = startx;
		}
		else
		{
			minx = startx;
			maxx = ev->x;
		}
		if(ev->y < starty)
		{
			miny = ev->y;
			maxy = starty;	
		}
		else
		{
			maxy = ev->y;
			miny = starty;
		}
	}
	else
	{
		//Should only be ev->x/y we are interested in
		if(ev->x < minx)
			minx = ev->x;
		if(ev->x > maxx)
			maxx = ev->x;
		if(ev->y < miny)
			miny = ev->y;
		if(ev->y > maxy)
			maxy = ev->y;
			
	}
	startx = ev->x;
	starty = ev->y;
	return 1;
}

static gboolean canvas_mouse_up(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	GdkPixmap *image = gdk_pixmap_new(NULL,maxx-minx,maxy-miny,24);
	GdkGC *gc = gdk_gc_new(image);
	GdkColor mycol = { 0, 65535,65535,65535 };
	gdk_gc_set_foreground(gc, &mycol);
	gdk_draw_rectangle(image,gc,1,0,0,maxx-minx,maxy-miny);
	gdk_draw_drawable(image,gdk_gc_new(image),canvas->window,minx,miny,0,0,maxx-minx+1, maxy-miny+1);
	check_pixmap(image);
	
	double width = maxx-minx;
	double height = maxy-miny;
	
	double boxw = width/3.0;
	double boxh = height/5.0;
	
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx,miny,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx+boxw,miny,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx+boxw*2,miny,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx,miny+boxh,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx+boxw,miny+boxh,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx+boxw*2,miny+boxh,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx,miny+boxh*2,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx+boxw,miny+boxh*2,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx+boxw*2,miny+boxh*2,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx,miny+boxh*3,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx+boxw,miny+boxh*3,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx+boxw*2,miny+boxh*3,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx,miny+boxh*4,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx+boxw,miny+boxh*4,boxw,boxh);
	gdk_draw_rectangle(canvas->window,gdk_gc_new(canvas->window),0,minx+boxw*2,miny+boxh*4,boxw,boxh);
	minx = 0;
	miny = 0;
	maxx = 0;
	maxy = 0;
	return 1;
}

static void canvas_clear(GtkWidget *canvas, gpointer user_data)
{
	
	cairo_t *cr = gdk_cairo_create(canvas->window);
	cairo_set_source_rgb(cr, 1,1,1);
	cairo_rectangle(cr,0,0,canvas->allocation.width,canvas->allocation.height);
	cairo_fill(cr);
	cairo_destroy(cr);
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	GtkWidget *window,*canvas, *clear, *vbox;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window),400,400);
	
	canvas = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(canvas),400,360);
	gtk_widget_add_events(canvas,GDK_BUTTON_MOTION_MASK|GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK);
	textbox = gtk_entry_new_with_max_length(100);
	
	clear = gtk_button_new_with_label("Clear");
		
	vbox = gtk_vbox_new(0,0);
	
	gtk_box_pack_start(GTK_BOX(vbox),canvas,0,0,0);
	gtk_box_pack_start(GTK_BOX(vbox),textbox,0,0,0);
	gtk_box_pack_start(GTK_BOX(vbox),clear,0,0,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_exit),NULL);
	g_signal_connect_swapped(G_OBJECT(clear),"clicked",G_CALLBACK(canvas_clear),canvas);
	
	g_signal_connect(G_OBJECT(canvas),"button-press-event",G_CALLBACK(canvas_mouse_down),NULL);
	g_signal_connect(G_OBJECT(canvas),"button-release-event",G_CALLBACK(canvas_mouse_up),NULL);
	g_signal_connect(G_OBJECT(canvas),"motion-notify-event",G_CALLBACK(canvas_mouse_move),NULL);
	
	gtk_widget_show_all(window);
	
	gtk_main();
}
