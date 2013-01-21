/* GTK - The GIMP Toolkit
 * Copyright(C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or(at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.	See the AUTHORS
 * file for a list of people on the GTK+ Team.	See the ChangeLog
 * files for a list of changes.	These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#include "gtkpmbutton.h"
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>

static void gtk_pmbutton_realize(GtkWidget *widget);
static void gtk_pmbutton_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_pmbutton_send_configure(GtkPMButton *pmbut);
static gboolean gtk_pmbutton_expose(GtkWidget *widget, GdkEventExpose *event);
static gboolean gtk_pmbutton_press(GtkWidget *widget, GdkEventButton *event);
static gboolean gtk_pmbutton_release(GtkWidget *widget, GdkEventButton   *event);
static gboolean gtk_pmbutton_hover(GtkWidget *widget, GdkEventCrossing *event);
static gboolean gtk_pmbutton_leave(GtkWidget *widget, GdkEventCrossing *event);

G_DEFINE_TYPE(GtkPMButton, gtk_pmbutton, GTK_TYPE_WIDGET)

static void gtk_pmbutton_class_init(GtkPMButtonClass *class)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);

	widget_class->realize = gtk_pmbutton_realize;
	widget_class->expose_event = gtk_pmbutton_expose;
	widget_class->size_allocate = gtk_pmbutton_size_allocate;
	widget_class->button_press_event = gtk_pmbutton_press;
	widget_class->button_release_event = gtk_pmbutton_release;
	widget_class->enter_notify_event = gtk_pmbutton_hover;
  	widget_class->leave_notify_event = gtk_pmbutton_leave;
  	g_signal_new ("clicked", G_TYPE_FROM_CLASS (class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, G_STRUCT_OFFSET (GtkPMButtonClass, pmbutton_clicked), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
	g_signal_new ("enter", G_TYPE_FROM_CLASS (class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, G_STRUCT_OFFSET (GtkPMButtonClass, pmbutton_hover), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
	g_signal_new ("leave", G_TYPE_FROM_CLASS (class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, G_STRUCT_OFFSET (GtkPMButtonClass, pmbutton_leave), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

static void gtk_pmbutton_init(GtkPMButton *pmbut)
{
	pmbut->state = 0;
}

GtkWidget* gtk_pmbutton_new(char *label)
{
	GtkWidget *button = g_object_new(GTK_TYPE_PMBUTTON,NULL);
	GtkPMButton *pmbutton = GTK_PMBUTTON(button);
	pmbutton->text = g_strdup(label);
	return button;
}

static void gtk_pmbutton_realize(GtkWidget *widget)
{
	GtkPMButton *pmbut;
	GdkWindowAttr attributes;
	gint attributes_mask;

	g_return_if_fail(GTK_IS_PMBUTTON(widget));

	pmbut = GTK_PMBUTTON(widget);
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

	gtk_pmbutton_send_configure(GTK_PMBUTTON(widget));
}

static void gtk_pmbutton_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
	g_return_if_fail(GTK_IS_PMBUTTON(widget));
	g_return_if_fail(allocation != NULL);

	widget->allocation = *allocation;

	if(GTK_WIDGET_REALIZED(widget))
	{
		gdk_window_move_resize(widget->window, allocation->x, allocation->y, allocation->width, allocation->height);
		gtk_pmbutton_send_configure(GTK_PMBUTTON(widget));
	}
}

static void gtk_pmbutton_send_configure(GtkPMButton *pmbut)
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

static gboolean gtk_pmbutton_expose(GtkWidget *widget, GdkEventExpose *event)
{
	GtkPMButton *pmbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMBUTTON (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	if (event->count > 0)
		return FALSE;
  
	pmbutton = GTK_PMBUTTON (widget);
	
	cairo_t *cr = gdk_cairo_create(widget->window);
	cairo_rectangle(cr,0,0,widget->allocation.width,widget->allocation.height);
	if(!pmbutton->state)
	{
		cairo_pattern_t* gradient = cairo_pattern_create_linear(0,0,0, widget->allocation.height);
		cairo_pattern_add_color_stop_rgb(gradient, 0.0,0.62,0.0,0.62);
		cairo_pattern_add_color_stop_rgb(gradient, 0.2,0.8,0.0,0.8);
		cairo_pattern_add_color_stop_rgb(gradient, 0.6,0.72,0.0,0.72);
		cairo_pattern_add_color_stop_rgb(gradient, 1.0,0.3,0.0,0.3);
		cairo_set_source(cr,gradient);
	}
	else if(pmbutton->state == 1)
	{
		cairo_pattern_t* gradient = cairo_pattern_create_linear(0,0,0, widget->allocation.height);
		cairo_pattern_add_color_stop_rgb(gradient, 1.0,0.62,0.0,0.62);
		cairo_pattern_add_color_stop_rgb(gradient, 0.6,0.8,0.0,0.8);
		cairo_pattern_add_color_stop_rgb(gradient, 0.2,0.72,0.0,0.72);
		cairo_pattern_add_color_stop_rgb(gradient, 0.0,0.3,0.0,0.3);
		cairo_set_source(cr,gradient);
	}
	else if(pmbutton->state == 2)
	{
		cairo_pattern_t* gradient = cairo_pattern_create_linear(0,0,0, widget->allocation.height);
		cairo_pattern_add_color_stop_rgb(gradient, 0.0,0.82,0.0,0.82);
		cairo_pattern_add_color_stop_rgb(gradient, 0.2,1.0,0.0,1.0);
		cairo_pattern_add_color_stop_rgb(gradient, 0.6,0.92,0.0,0.92);
		cairo_pattern_add_color_stop_rgb(gradient, 1.0,0.5,0.0,0.5);
		cairo_set_source(cr,gradient);
	}
	cairo_fill_preserve(cr);
	cairo_set_source_rgb(cr,0,0,0);
	cairo_set_line_width(cr,5);
	cairo_stroke(cr);
	cairo_save(cr);
	cairo_set_source_rgb(cr,1,1,1);
	cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, 30);
	cairo_text_extents_t exts;
	cairo_text_extents(cr,pmbutton->text,&exts);
	double scale_am = (widget->allocation.width-50)/exts.width;
	cairo_move_to (cr, widget->allocation.width/2-(exts.width*scale_am)/2,widget->allocation.height/2+(exts.height*scale_am)/3);
	cairo_scale(cr,scale_am,scale_am);
	cairo_show_text (cr, pmbutton->text);
	
	cairo_fill(cr);
	cairo_restore(cr);
	cairo_destroy(cr);
	return FALSE;
}

static gboolean gtk_pmbutton_press(GtkWidget *widget, GdkEventButton *event)
{
	GtkPMButton *pmbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMBUTTON(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	pmbutton = GTK_PMBUTTON(widget);
 
	if (pmbutton->state != 1) //KEYWORD - I love dilbert
	{
		gtk_grab_add (widget);
		pmbutton->state = 1;
		gtk_widget_queue_draw(widget);
	}
	return FALSE;
}

static gboolean gtk_pmbutton_release(GtkWidget *widget, GdkEventButton *event)
{
	GtkPMButton *pmbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMBUTTON(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	pmbutton = GTK_PMBUTTON(widget);
	gtk_grab_remove (widget);
	if (pmbutton->state)
	{
		pmbutton->state = 0; 
		if(event->x > 0 && event->x < widget->allocation.width && event->y > 0 && event->y < widget->allocation.height)
		{
			gtk_signal_emit_by_name(GTK_OBJECT(widget),"clicked");
			pmbutton->state = 2;
		}

		gtk_widget_queue_draw(widget);
	}
	return FALSE;
}

static gboolean gtk_pmbutton_hover(GtkWidget *widget, GdkEventCrossing *event)
{
	GtkPMButton *pmbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMBUTTON(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	pmbutton = GTK_PMBUTTON(widget);
	gtk_signal_emit_by_name(GTK_OBJECT(widget),"enter");
	pmbutton->state = 2;
	gtk_widget_queue_draw(widget);
	return FALSE;
}
static gboolean gtk_pmbutton_leave(GtkWidget *widget, GdkEventCrossing *event)
{
	GtkPMButton *pmbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMBUTTON(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	pmbutton = GTK_PMBUTTON(widget);
	gtk_signal_emit_by_name(GTK_OBJECT(widget),"leave");
	pmbutton->state = 0;
	gtk_widget_queue_draw(widget);
	return FALSE;
}

#define __GTK_PMBUTTON_C__
