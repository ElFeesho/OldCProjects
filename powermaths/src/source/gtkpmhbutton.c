#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include "gtkpmhbutton.h"
#include "misc.h"

static void gtk_pmhbutton_realize(GtkWidget *widget);
static void gtk_pmhbutton_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_pmhbutton_send_configure(GtkPMHButton *pmbut);
static gboolean gtk_pmhbutton_expose(GtkWidget *widget, GdkEventExpose *event);
static gboolean gtk_pmhbutton_press(GtkWidget *widget, GdkEventButton *event);
static gboolean gtk_pmhbutton_release(GtkWidget *widget, GdkEventButton   *event);
static gboolean gtk_pmhbutton_hover(GtkWidget *widget, GdkEventCrossing *event);
static gboolean gtk_pmhbutton_leave(GtkWidget *widget, GdkEventCrossing *event);

G_DEFINE_TYPE(GtkPMHButton, gtk_pmhbutton, GTK_TYPE_WIDGET)

static void gtk_pmhbutton_class_init(GtkPMHButtonClass *class)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);

	widget_class->realize = gtk_pmhbutton_realize;
	widget_class->expose_event = gtk_pmhbutton_expose;
	widget_class->size_allocate = gtk_pmhbutton_size_allocate;
	widget_class->button_press_event = gtk_pmhbutton_press;
	widget_class->button_release_event = gtk_pmhbutton_release;
	widget_class->enter_notify_event = gtk_pmhbutton_hover;
  	widget_class->leave_notify_event = gtk_pmhbutton_leave;
  	g_signal_new ("clicked", G_TYPE_FROM_CLASS (class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, G_STRUCT_OFFSET (GtkPMHButtonClass, pmhbutton_clicked), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
	g_signal_new ("enter", G_TYPE_FROM_CLASS (class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, G_STRUCT_OFFSET (GtkPMHButtonClass, pmhbutton_hover), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
	g_signal_new ("leave", G_TYPE_FROM_CLASS (class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, G_STRUCT_OFFSET (GtkPMHButtonClass, pmhbutton_leave), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

static void gtk_pmhbutton_init(GtkPMHButton *pmbut)
{
	pmbut->state = 0;
}

GtkWidget* gtk_pmhbutton_new(char *label)
{
	GtkWidget *button = g_object_new(GTK_TYPE_PMHBUTTON,NULL);
	GtkPMHButton *pmhbutton = GTK_PMHBUTTON(button);
	pmhbutton->text = g_strdup(label);
	//gtk_widget_modify_bg(button,GTK_STATE_NORMAL,misc_create_color(65535,65535,65535));
	return button;
}

static void gtk_pmhbutton_realize(GtkWidget *widget)
{
	GtkPMHButton *pmbut;
	GdkWindowAttr attributes;
	gint attributes_mask;

	g_return_if_fail(GTK_IS_PMHBUTTON(widget));

	pmbut = GTK_PMHBUTTON(widget);
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
	gdk_window_set_user_data(widget->window, pmbut);

	widget->style = gtk_style_attach(widget->style, widget->window);
	gtk_style_set_background(widget->style, widget->window, GTK_STATE_NORMAL);

	gtk_pmhbutton_send_configure(GTK_PMHBUTTON(widget));
}

static void gtk_pmhbutton_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
	g_return_if_fail(GTK_IS_PMHBUTTON(widget));
	g_return_if_fail(allocation != NULL);

	widget->allocation = *allocation;

	if(GTK_WIDGET_REALIZED(widget))
	{
		widget->allocation = *allocation;
		gdk_window_move_resize(widget->window, allocation->x, allocation->y, allocation->width, allocation->height);
		gtk_pmhbutton_send_configure(GTK_PMHBUTTON(widget));
	}
}

static void gtk_pmhbutton_send_configure(GtkPMHButton *pmbut)
{
	GtkWidget *widget;
	GdkEvent *event = gdk_event_new(GDK_CONFIGURE);

	widget = GTK_WIDGET(pmbut);

	event->configure.window = g_object_ref(widget->window);
	event->configure.send_event = TRUE;
	event->configure.x = widget->allocation.x;
	event->configure.y = widget->allocation.y;
	event->configure.width = widget->allocation.width;
	event->configure.height = widget->allocation.height;
	
	gtk_widget_event(widget, event);
	gdk_event_free(event);
}

static gboolean gtk_pmhbutton_expose(GtkWidget *widget, GdkEventExpose *event)
{
	GtkPMHButton *pmhbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMHBUTTON (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	if (event->count > 0)
		return FALSE;

	pmhbutton = GTK_PMHBUTTON (widget);
	
	cairo_t *cr = gdk_cairo_create(widget->window);
	cairo_rectangle(cr,0,0,widget->allocation.width,widget->allocation.height);
	cairo_set_source_rgb(cr,42405.0/65536.0,0,8481/65536.0);
	cairo_fill(cr);
	/* Draw a nice rounded rectangle! Corners have a radius of 20 */
	int radius = 20;
	int w = widget->allocation.width-10;
	int h = widget->allocation.height-10;
	int offx = 5, offy = 5;
	cairo_move_to(cr,0+offx,radius+offy);
	cairo_curve_to(cr,0+offx,radius+offy,0+offx,0+offy,radius+offx,0+offy);
	cairo_line_to(cr,w-radius+offx,0+offy);
	cairo_curve_to(cr,w-radius+offx,0+offy,w+offx,0+offy,w+offx,radius+offy);
	cairo_line_to(cr,w+offx,h-radius+offy);
	cairo_curve_to(cr,w+offx,h+offy-radius,w+offx,h+offy,w+offx-radius,h+offy);
	cairo_line_to(cr,offx+radius,h+offy);
	cairo_curve_to(cr,offx+radius,h+offx,offx,h+offx,offx,h+offx-radius);
	cairo_line_to(cr,offx,radius+offy);
	if(!pmhbutton->state)
	{
		
		cairo_pattern_t* gradient = cairo_pattern_create_linear(0,0,0, widget->allocation.height);
		cairo_pattern_add_color_stop_rgb(gradient, 0.0,0.64,0.0,0.12);
		cairo_pattern_add_color_stop_rgb(gradient, 0.2,0.84,0.0,0.32);
		cairo_pattern_add_color_stop_rgb(gradient, 0.6,0.74,0.0,0.22);
		cairo_pattern_add_color_stop_rgb(gradient, 1.0,0.54,0.0,0.12);
		cairo_set_source(cr,gradient);
		
	}
	else if(pmhbutton->state == 1)
	{
		
		cairo_pattern_t* gradient = cairo_pattern_create_linear(0,0,0, widget->allocation.height);
		cairo_pattern_add_color_stop_rgb(gradient, 1.0,0.64,0.0,0.12);
		cairo_pattern_add_color_stop_rgb(gradient, 0.6,0.84,0.0,0.32);
		cairo_pattern_add_color_stop_rgb(gradient, 0.2,0.74,0.0,0.22);
		cairo_pattern_add_color_stop_rgb(gradient, 0.0,0.54,0.0,0.12);
		cairo_set_source(cr,gradient);
		
	}
	else if(pmhbutton->state == 2)
	{
		cairo_pattern_t* gradient = cairo_pattern_create_linear(0,0,0, widget->allocation.height);
		cairo_pattern_add_color_stop_rgb(gradient, 0.0,0.74,0.0,0.22);
		cairo_pattern_add_color_stop_rgb(gradient, 0.2,0.94,0.0,0.42);
		cairo_pattern_add_color_stop_rgb(gradient, 0.6,0.84,0.0,0.32);
		cairo_pattern_add_color_stop_rgb(gradient, 1.0,0.64,0.0,0.22);
		cairo_set_source(cr,gradient);
	}
	cairo_fill_preserve(cr);
	cairo_set_source_rgb(cr,1,0.82,0.0);
	cairo_set_line_width(cr,5);
	cairo_stroke(cr);
	
	cairo_save(cr);
	cairo_set_source_rgb(cr,1,0.82,0.0);
	cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, 30);
	
	cairo_text_extents_t exts;
	cairo_text_extents(cr,pmhbutton->text,&exts);
	double scale_amx = (widget->allocation.width-50)/exts.width;
	double scale_amy = (widget->allocation.height-40)/exts.height;
	cairo_move_to (cr, widget->allocation.width/2-(exts.width*MIN(scale_amx,scale_amy))/2,widget->allocation.height/2+(exts.height*MIN(scale_amx,scale_amy))/2.2);
	cairo_scale(cr,MIN(scale_amx,scale_amy),MIN(scale_amx,scale_amy));
	cairo_show_text (cr, pmhbutton->text);
	
	cairo_fill(cr);
	cairo_restore(cr);
	cairo_destroy(cr);
	return FALSE;
}

static gboolean gtk_pmhbutton_press(GtkWidget *widget, GdkEventButton *event)
{
	GtkPMHButton *pmhbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMHBUTTON(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	pmhbutton = GTK_PMHBUTTON(widget);
 
	if (pmhbutton->state != 1) //KEYWORD - I love dilbert
	{
		gtk_grab_add (widget);
		pmhbutton->state = 1;
		gtk_widget_queue_draw(widget);
	}
	return FALSE;
}

static gboolean gtk_pmhbutton_release(GtkWidget *widget, GdkEventButton *event)
{
	GtkPMHButton *pmhbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMHBUTTON(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	pmhbutton = GTK_PMHBUTTON(widget);
	gtk_grab_remove (widget);
	if (pmhbutton->state)
	{
		pmhbutton->state = 0; 
		if(event->x > 0 && event->x < widget->allocation.width && event->y > 0 && event->y < widget->allocation.height)
		{
			gtk_signal_emit_by_name(GTK_OBJECT(widget),"clicked");
			pmhbutton->state = 2;
		}

		gtk_widget_queue_draw(widget);
	}
	return FALSE;
}

static gboolean gtk_pmhbutton_hover(GtkWidget *widget, GdkEventCrossing *event)
{
	GtkPMHButton *pmhbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMHBUTTON(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	pmhbutton = GTK_PMHBUTTON(widget);
	gtk_signal_emit_by_name(GTK_OBJECT(widget),"enter");
	pmhbutton->state = 2;
	gtk_widget_queue_draw(widget);
	return FALSE;
}
static gboolean gtk_pmhbutton_leave(GtkWidget *widget, GdkEventCrossing *event)
{
	GtkPMHButton *pmhbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMHBUTTON(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	pmhbutton = GTK_PMHBUTTON(widget);
	gtk_signal_emit_by_name(GTK_OBJECT(widget),"leave");
	pmhbutton->state = 0;
	gtk_widget_queue_draw(widget);
	return FALSE;
}

#define __GTK_PMHBUTTON_C__
