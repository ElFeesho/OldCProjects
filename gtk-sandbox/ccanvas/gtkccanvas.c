#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>
#include "gtkccanvas.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

static void gtk_ccanvas_realize(GtkWidget *widget);
static void gtk_ccanvas_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_ccanvas_send_configure(GtkCCanvas *ccanv);
static gboolean gtk_ccanvas_expose(GtkWidget *widget, GdkEventExpose *event);
static gboolean gtk_ccanvas_motion(GtkWidget *widget, GdkEventMotion *event);
static gboolean gtk_ccanvas_press(GtkWidget *widget, GdkEventButton *event);
static gboolean gtk_ccanvas_release(GtkWidget *widget, GdkEventButton   *event);
static gboolean gtk_ccanvas_hover(GtkWidget *widget, GdkEventCrossing *event);
static gboolean gtk_ccanvas_leave(GtkWidget *widget, GdkEventCrossing *event);

G_DEFINE_TYPE(GtkCCanvas, gtk_ccanvas, GTK_TYPE_WIDGET)

static void gtk_ccanvas_class_init(GtkCCanvasClass *class)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
	widget_class->realize = gtk_ccanvas_realize;
	widget_class->expose_event = gtk_ccanvas_expose;
	widget_class->size_allocate = gtk_ccanvas_size_allocate;
	widget_class->button_press_event = gtk_ccanvas_press;
	widget_class->button_release_event = gtk_ccanvas_release;
	widget_class->motion_notify_event = gtk_ccanvas_motion;
	widget_class->enter_notify_event = gtk_ccanvas_hover;
  	widget_class->leave_notify_event = gtk_ccanvas_leave;
}

static void gtk_ccanvas_init(GtkCCanvas *ccanv)
{
	ccanv->brush_size = 1;
	ccanv->incanv = 0;
	ccanv->show_outline = 1;
}

GtkWidget* gtk_ccanvas_new()
{
	GtkWidget *canvas = g_object_new(GTK_TYPE_CCANVAS,NULL);
	return canvas;
}

GtkWidget* gtk_ccanvas_new_with_border()
{
	GtkWidget *canvas = g_object_new(GTK_TYPE_CCANVAS,NULL);
	GtkCCanvas *ccanvas = GTK_CCANVAS(canvas);
	ccanvas->border = 1;
	return canvas;
}

gboolean gtk_ccanvas_set_outline(GtkCCanvas *ccanvas, gboolean outline)
{
	g_return_val_if_fail (ccanvas != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS(ccanvas), FALSE);
	
	ccanvas->show_outline = outline;
	return TRUE;
}

gboolean gtk_ccanvas_get_outline(GtkCCanvas *ccanvas)
{
	g_return_val_if_fail (ccanvas != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS(ccanvas), FALSE);
	
	return ccanvas->show_outline;
}

gboolean gtk_ccanvas_set_brush_size(GtkCCanvas *ccanvas, guint size)
{
	g_return_val_if_fail (ccanvas != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS(ccanvas), FALSE);
	ccanvas->brush_size = size;
	return TRUE;
}

gint gtk_ccanvas_get_brush_size(GtkCCanvas *ccanvas)
{
	g_return_val_if_fail (ccanvas != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS(ccanvas), FALSE);
	
	return ccanvas->show_outline;
}

gboolean gtk_ccanvas_set_border(GtkCCanvas *ccanvas, gboolean border)
{
	g_return_val_if_fail (ccanvas != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS(ccanvas), FALSE);
	
	ccanvas->border = border;
	return TRUE;
}

gboolean gtk_ccanvas_get_border(GtkCCanvas *ccanvas)
{
	g_return_val_if_fail (ccanvas != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS(ccanvas), FALSE);
	
	return ccanvas->border;
}

static void gtk_ccanvas_realize(GtkWidget *widget)
{
	GtkCCanvas *ccanv;
	GdkWindowAttr attributes;
	gint attributes_mask;

	g_return_if_fail(GTK_IS_CCANVAS(widget));

	ccanv = GTK_CCANVAS(widget);
	GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);

	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.visual = gtk_widget_get_visual(widget);
	attributes.colormap = gtk_widget_get_colormap(widget);
	attributes.event_mask = gtk_widget_get_events(widget) | GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK | GDK_POINTER_MOTION_MASK | GDK_LEAVE_NOTIFY_MASK;

	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

	widget->window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask);
	gdk_window_set_user_data(widget->window, ccanv);

	widget->style = gtk_style_attach(widget->style, widget->window);
	gtk_style_set_background(widget->style, widget->window, GTK_STATE_NORMAL);

	ccanv->back_buffer = gdk_pixmap_new(ccanv->widget.window,ccanv->widget.parent->allocation.width, ccanv->widget.parent->allocation.height, 24);
	GdkGC *gc = gdk_gc_new(ccanv->back_buffer);
	GdkColor white;
	gdk_color_parse("white",&white);
	gdk_gc_set_rgb_fg_color(gc, &white);
	gdk_draw_rectangle(ccanv->back_buffer,gc,1,0,0,ccanv->widget.allocation.width, ccanv->widget.allocation.height);
	g_object_unref(gc);
	gtk_ccanvas_send_configure(GTK_CCANVAS(widget));
}

static void gtk_ccanvas_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
	g_return_if_fail(GTK_IS_CCANVAS(widget));
	g_return_if_fail(allocation != NULL);

	widget->allocation = *allocation;
	GtkCCanvas *ccanvas = GTK_CCANVAS (widget);
	if(GTK_WIDGET_REALIZED(widget))
	{
		int bbwidth, bbheight;
		gdk_drawable_get_size(ccanvas->back_buffer,&bbwidth, &bbheight);
		
		if(allocation->width != bbwidth || allocation->height != bbheight)
		{
			//Create a new backbuffer, for the widget since the size has changed!
			GdkPixmap *bbuf = gdk_pixmap_new(widget->window, allocation->width, allocation->height, -1);
			GdkGC *gc = gdk_gc_new(bbuf);
			GdkColor white;
			gdk_color_parse("white",&white);
			gdk_gc_set_rgb_fg_color(gc,&white);
			gdk_draw_rectangle(bbuf,gc,1,0,0,ccanvas->widget.allocation.width, ccanvas->widget.allocation.height);
			gdk_draw_drawable(bbuf,gdk_gc_new(bbuf),ccanvas->back_buffer,0,0,0,0,bbwidth,bbheight);
			ccanvas->back_buffer = bbuf;
			g_object_unref(gc);
		}
		gdk_window_move_resize(widget->window, allocation->x, allocation->y, allocation->width, allocation->height);
		gtk_ccanvas_send_configure(GTK_CCANVAS(widget));
	}
}

static void gtk_ccanvas_send_configure(GtkCCanvas *ccanv)
{
	GtkWidget *widget;
	GdkEvent *event = gdk_event_new(GDK_CONFIGURE);

	widget = GTK_WIDGET(ccanv);

	event->configure.window = g_object_ref(widget->window);
	event->configure.send_event = TRUE;
	event->configure.x = widget->allocation.x;
	event->configure.y = widget->allocation.y;
	event->configure.width = widget->allocation.width;
	event->configure.height = widget->allocation.height;
	
	gtk_widget_event(widget, event);
	gdk_event_free(event);
}

static gboolean gtk_ccanvas_expose(GtkWidget *widget, GdkEventExpose *event)
{
	GtkCCanvas *ccanvas;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	if (event->count > 0)
		return FALSE;
  
	ccanvas = GTK_CCANVAS (widget);
	GdkGC *gc = gdk_gc_new(widget->window);
	gdk_draw_drawable(widget->window,gc,ccanvas->back_buffer,event->area.x,event->area.y,event->area.x,event->area.y,event->area.width, event->area.height);
	g_object_unref(gc);
	
	if(ccanvas->show_outline && ccanvas->incanv)
	{
		gc = gdk_gc_new(widget->window);
		gdk_gc_set_function(gc,GDK_INVERT);
		GdkColor black;
		gdk_color_parse("black",&black);
		gdk_gc_set_rgb_fg_color(gc,&black);
		gdk_draw_rectangle(widget->window,gc,FALSE,ccanvas->mx-(ccanvas->brush_size/2+1),ccanvas->my-(ccanvas->brush_size/2+1),ccanvas->brush_size+1,ccanvas->brush_size+1);
		g_object_unref(gc);
	}
	if(ccanvas->border)
	{
		gc = gdk_gc_new(widget->window);
		GdkColor black;
		gdk_color_parse("black",&black);
		gdk_gc_set_rgb_fg_color(gc,&black);
		gdk_draw_rectangle(widget->window,gc,FALSE,0,0,widget->allocation.width-1,widget->allocation.height-1);
		g_object_unref(gc);
	}
	return FALSE;
}

static gboolean gtk_ccanvas_motion(GtkWidget *widget, GdkEventMotion *event)
{
	GtkCCanvas *ccanvas;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	ccanvas = GTK_CCANVAS(widget);
	ccanvas->mx = event->x;
	ccanvas->my = event->y;
	if((event->state & GDK_BUTTON1_MASK)==GDK_BUTTON1_MASK)
	{
		if(ccanvas->toolmode == GTK_CCANVAS_FREEH)
		{
			cairo_t *cr = gdk_cairo_create(ccanvas->back_buffer);
			cairo_set_line_width(cr,ccanvas->brush_size);
			cairo_set_line_cap(cr,CAIRO_LINE_CAP_ROUND);
			cairo_move_to(cr, ccanvas->ox, ccanvas->oy);
			cairo_line_to(cr,ccanvas->mx, ccanvas->my);
			cairo_stroke(cr);
			cairo_destroy(cr);
		}
	}
	gtk_widget_queue_draw(widget);
	ccanvas->ox = event->x;
	ccanvas->oy = event->y;
	
	return FALSE;
}

static gboolean gtk_ccanvas_press(GtkWidget *widget, GdkEventButton *event)
{
	GtkCCanvas *ccanvas;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	ccanvas = GTK_CCANVAS(widget);
	ccanvas->ox = event->x;
	ccanvas->oy = event->y;
	ccanvas->mdown = 1;
	if(event->button == 1)
	{
		if(ccanvas->toolmode == GTK_CCANVAS_FREEH)
		{
			cairo_t *cr = gdk_cairo_create(ccanvas->back_buffer);
			cairo_set_line_width(cr,ccanvas->brush_size);
			cairo_set_line_cap(cr,CAIRO_LINE_CAP_ROUND);
			cairo_move_to(cr, ccanvas->ox, ccanvas->oy);
			cairo_line_to(cr,ccanvas->mx, ccanvas->my);
			cairo_stroke(cr);
			cairo_destroy(cr);
		}
	}
	gtk_grab_add (widget);
	gtk_widget_queue_draw(widget);
	return FALSE;
}

static gboolean gtk_ccanvas_release(GtkWidget *widget, GdkEventButton *event)
{
	GtkCCanvas *ccanvas;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	ccanvas = GTK_CCANVAS(widget);
	ccanvas->mdown = 0;
	gtk_grab_remove (widget);
	gtk_widget_queue_draw(widget);
	return FALSE;
}

static gboolean gtk_ccanvas_hover(GtkWidget *widget, GdkEventCrossing *event)
{
	GtkCCanvas *ccanvas;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	ccanvas = GTK_CCANVAS(widget);
	ccanvas->incanv = 1;
	return FALSE;
}
static gboolean gtk_ccanvas_leave(GtkWidget *widget, GdkEventCrossing *event)
{
	GtkCCanvas *ccanvas;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_CCANVAS(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	ccanvas = GTK_CCANVAS(widget);
	ccanvas->incanv = 0;
	gtk_widget_queue_draw(widget);
	return FALSE;
}

#define __GTK_CCANVAS_C__
