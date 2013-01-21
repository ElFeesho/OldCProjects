#include <gtk/gtk.h>
#include "gtkvimage.h"

static void gtk_vimage_realize(GtkWidget *widget);
static void gtk_vimage_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_vimage_send_configure(GtkVImage *vimg);
static gboolean gtk_vimage_expose(GtkWidget *widget, GdkEventExpose *event);

G_DEFINE_TYPE(GtkVImage, gtk_vimage, GTK_TYPE_WIDGET)

static void gtk_vimage_class_init(GtkVImageClass *class)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);

	widget_class->realize = gtk_vimage_realize;
	widget_class->expose_event = gtk_vimage_expose;
	widget_class->size_allocate = gtk_vimage_size_allocate;
}

static void gtk_vimage_init(GtkVImage *vimg)
{

}

GtkWidget* gtk_vimage_new(char *file)
{
	GtkWidget *button = g_object_new(GTK_TYPE_VIMAGE,NULL);
	GtkVImage *vimage = GTK_VIMAGE(button);
	vimage->file = rsvg_handle_new_from_file(file,NULL);
	RsvgDimensionData dimensions;
	rsvg_handle_get_dimensions(vimage->file, &dimensions);
	vimage->width = dimensions.width;
	vimage->height = dimensions.height;
	return button;
}

static void gtk_vimage_realize(GtkWidget *widget)
{
	GtkVImage *vimg;
	GdkWindowAttr attributes;
	gint attributes_mask;

	g_return_if_fail(GTK_IS_VIMAGE(widget));

	vimg = GTK_VIMAGE(widget);
	GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);

	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.visual = gtk_widget_get_visual(widget);
	attributes.colormap = gtk_widget_get_colormap(widget);
	attributes.event_mask = gtk_widget_get_events(widget) | GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK;

	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

	widget->window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask);
	gdk_window_set_user_data(widget->window, vimg);

	widget->style = gtk_style_attach(widget->style, widget->window);
	gtk_style_set_background(widget->style, widget->window, GTK_STATE_NORMAL);

	gtk_vimage_send_configure(GTK_VIMAGE(widget));
}

static void gtk_vimage_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
	g_return_if_fail(GTK_IS_VIMAGE(widget));
	g_return_if_fail(allocation != NULL);

	widget->allocation = *allocation;

	if(GTK_WIDGET_REALIZED(widget))
	{
		gdk_window_move_resize(widget->window, allocation->x, allocation->y, allocation->width, allocation->height);
		gtk_vimage_send_configure(GTK_VIMAGE(widget));
	}
}

static void gtk_vimage_send_configure(GtkVImage *vimg)
{
	GtkWidget *widget;
	GdkEvent *event = gdk_event_new(GDK_CONFIGURE);

	widget = GTK_WIDGET(vimg);

	event->configure.window = g_object_ref(widget->window);
	event->configure.send_event = TRUE;
	event->configure.x = widget->allocation.x;
	event->configure.y = widget->allocation.y;
	event->configure.width = widget->allocation.width;
	event->configure.height = widget->allocation.height;
	
	gtk_widget_event(widget, event);
	gdk_event_free(event);
}

static gboolean gtk_vimage_expose(GtkWidget *widget, GdkEventExpose *event)
{
	GtkVImage *vimage;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_VIMAGE (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	if (event->count > 0)
		return FALSE;
  
	vimage = GTK_VIMAGE (widget);
	
	cairo_t *cr = gdk_cairo_create(widget->window);
	if(widget->allocation.width > widget->allocation.height)
	{
		cairo_scale(cr, ((double)widget->allocation.height)/((double)vimage->width), ((double)widget->allocation.height)/((double)vimage->height));
	}
	if(widget->allocation.width < widget->allocation.height)
	{
		cairo_scale(cr, ((double)widget->allocation.width)/((double)vimage->width), ((double)widget->allocation.width)/((double)vimage->height));
	}
	rsvg_handle_render_cairo(vimage->file,cr);
	cairo_destroy(cr);
	return FALSE;
}

#define __GTK_VIMAGE_C__
