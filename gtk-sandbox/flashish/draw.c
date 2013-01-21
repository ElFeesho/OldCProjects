#include <gtk/gtk.h>

static GtkWidget *canvas = NULL;
static int sx = -1, sy = -1;
static int lx = 0, ly = 0;

static GdkGC *canvas_gc = NULL;
static GdkGC *canvas_buf_gc = NULL;
static GdkPixmap *canvas_buf = NULL;

void canvas_init(GtkWidget *canvas)
{
	if(canvas_gc != NULL)
	{
		g_object_unref(canvas_gc);
	}
	canvas_gc = gdk_gc_new(canvas->window);


	if(canvas_buf != NULL)
	{
		GdkPixmap *temp = gdk_pixmap_new(canvas->window, canvas->allocation.width, canvas->allocation.height, -1);
		if(canvas_buf_gc != NULL)
			g_object_unref(canvas_buf_gc);
		canvas_buf_gc = gdk_gc_new(temp);

		GdkColor white;
		gdk_color_parse("white",&white);
		gdk_gc_set_rgb_fg_color(canvas_buf_gc, &white);
		gdk_draw_rectangle(temp, canvas_buf_gc, TRUE, 0, 0, canvas->allocation.width, canvas->allocation.height);
		gdk_draw_drawable(temp, canvas_buf_gc, canvas_buf, 0, 0, 0, 0, -1, -1);
		g_object_unref(canvas_buf);
		canvas_buf = temp;
	}
	else
	{
		canvas_buf = gdk_pixmap_new(canvas->window, canvas->allocation.width, canvas->allocation.height, -1);
		if(canvas_buf_gc != NULL)
			g_object_unref(canvas_buf_gc);
		canvas_buf_gc = gdk_gc_new(canvas_buf);

		GdkColor white;
		gdk_color_parse("white",&white);
		gdk_gc_set_rgb_fg_color(canvas_buf_gc, &white);
		gdk_draw_rectangle(canvas_buf, canvas_buf_gc, TRUE, 0, 0, canvas->allocation.width, canvas->allocation.height);
	}

	gtk_widget_queue_draw(canvas);
}

gboolean canvas_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer userdata)
{
	printf("DEBUG: Canvas Expose\n");
	if(canvas_buf == NULL)
	{
		canvas_init(canvas);
		return TRUE;
	}

	gdk_draw_drawable(canvas->window, canvas_gc, canvas_buf,0,0,0,0,-1,-1);

	return TRUE;
}

gboolean canvas_click(GtkWidget *canvas, GdkEventButton *ev, gpointer userdata)
{
	printf("DEBUG: Canvas Click\n");
	sx = ev->x;
	sy = ev->y;
	return TRUE;
}

gboolean canvas_motion(GtkWidget *canvas, GdkEventMotion *ev, gpointer userdata)
{
	if(sx == -1 && sy == -1)
		return TRUE;
//	printf("DEBUG: Canvas Motion\n");

	// Be clever soon... Draw only what needs to be replaced.
	gdk_draw_drawable(canvas->window, canvas_gc, canvas_buf,0,0,0,0,-1,-1);
	GdkColor black;
	gdk_color_parse("black", &black);
	gdk_gc_set_rgb_fg_color(canvas_gc, &black);

	gdk_gc_set_function(canvas_gc, GDK_INVERT);

	gdk_draw_line(canvas->window, canvas_gc, sx, sy, ev->x, ev->y);

	gdk_gc_set_function(canvas_gc, GDK_COPY);

	lx = ev->x;
	ly = ev->y;

	return TRUE;
}

gboolean canvas_release(GtkWidget *canvas, GdkEventButton *ev, gpointer userdata)
{
	printf("DEBUG: Canvas Release\n");
	gdk_draw_drawable(canvas->window, canvas_gc, canvas_buf,0,0,0,0,-1,-1);

	cairo_t *cr = gdk_cairo_create(canvas->window);

	cairo_move_to(cr, sx, sy);

	cairo_line_to(cr, ev->x, ev->y);

	cairo_stroke(cr);

	cairo_destroy(cr);

	gdk_draw_drawable(canvas_buf, canvas_buf_gc, canvas->window,0,0,0,0,-1,-1);

	sx = -1;
	sy = -1;

	return TRUE;
}

void canvas_resize(GtkWidget *canvas, GtkAllocation *alloc, gpointer userdata)
{
	printf("DEBUG: Canvas Resize\n");
	if(canvas->window == NULL)
		return;

	canvas_init(canvas);
}


GtkWidget *canvas_create()
{
	if(canvas!=NULL)
		return canvas;

	canvas = gtk_drawing_area_new();
	gtk_widget_set_events(canvas, gtk_widget_get_events(canvas) | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_BUTTON_MOTION_MASK | GDK_POINTER_MOTION_MASK);
	g_signal_connect(G_OBJECT(canvas), "expose-event", G_CALLBACK(canvas_expose), NULL);
	g_signal_connect(G_OBJECT(canvas), "button-press-event", G_CALLBACK(canvas_click), NULL);
	g_signal_connect(G_OBJECT(canvas), "button-release-event", G_CALLBACK(canvas_release), NULL);
	g_signal_connect(G_OBJECT(canvas), "motion-notify-event", G_CALLBACK(canvas_motion), NULL);
	g_signal_connect(G_OBJECT(canvas), "size-allocate", G_CALLBACK(canvas_resize), NULL);

	return canvas;
}

