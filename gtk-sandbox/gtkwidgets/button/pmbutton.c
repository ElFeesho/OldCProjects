#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>

#include "pmbutton.h"

/* Forward declarations */

static void gtk_pmbutton_class_init(GtkPMButtonClass *klass);
static void gtk_pmbutton_init(GtkPMButton *pmbutton);
static void gtk_pmbutton_destroy(GtkObject *object);
static void gtk_pmbutton_realize(GtkWidget *widget);
static gboolean gtk_pmbutton_draw(GtkWidget *widget);
static void gtk_pmbutton_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_pmbutton_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static gboolean gtk_pmbutton_expose(GtkWidget *widget, GdkEventExpose *event);
static gboolean gtk_pmbutton_press(GtkWidget *widget, GdkEventButton *event);
static gboolean gtk_pmbutton_release(GtkWidget *widget, GdkEventButton   *event);
static gboolean gtk_pmbutton_hover(GtkWidget *widget, GdkEventCrossing *event);
static gboolean gtk_pmbutton_leave(GtkWidget *widget, GdkEventCrossing *event);


static GtkWidgetClass *parent_class = NULL;

GType gtk_pmbutton_get_type ()
{
  static GType pmbutton_type = 0;

	if (!pmbutton_type)
	{
		static const GTypeInfo pmbutton_info =
		{
			sizeof (GtkPMButtonClass),
			NULL,
			NULL,
			(GClassInitFunc) gtk_pmbutton_class_init,
			NULL,
			NULL,
			sizeof (GtkPMButton),
       		0,
			(GInstanceInitFunc) gtk_pmbutton_init,
		};

		pmbutton_type = g_type_register_static (GTK_TYPE_WIDGET, "GtkPMButton", &pmbutton_info, 0);
	}

	return pmbutton_type;
}

static void gtk_pmbutton_class_init (GtkPMButtonClass *class)
{
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;

	object_class = (GtkObjectClass*) class;
	widget_class = (GtkWidgetClass*) class;

	parent_class = gtk_type_class (gtk_widget_get_type ());

	object_class->destroy = gtk_pmbutton_destroy;

	g_signal_new ("clicked", G_TYPE_FROM_CLASS (class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, G_STRUCT_OFFSET (GtkPMButtonClass, pmbutton_clicked), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
	g_signal_new ("enter", G_TYPE_FROM_CLASS (class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, G_STRUCT_OFFSET (GtkPMButtonClass, pmbutton_hover), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
	g_signal_new ("leave", G_TYPE_FROM_CLASS (class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, G_STRUCT_OFFSET (GtkPMButtonClass, pmbutton_leave), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	widget_class->realize = gtk_pmbutton_realize;
	widget_class->expose_event = gtk_pmbutton_expose;
	widget_class->size_request = gtk_pmbutton_size_request;
	widget_class->size_allocate = gtk_pmbutton_size_allocate;
	widget_class->button_press_event = gtk_pmbutton_press;
	widget_class->button_release_event = gtk_pmbutton_release;
	widget_class->enter_notify_event = gtk_pmbutton_hover;
  	widget_class->leave_notify_event = gtk_pmbutton_leave;
}

static void gtk_pmbutton_init (GtkPMButton *pmbutton)
{
	pmbutton->state = 0;
	pmbutton->images[0] = rsvg_handle_new_from_file("button.svg",NULL);
	pmbutton->images[1] = rsvg_handle_new_from_file("button_down.svg",NULL);
	pmbutton->oldw = 0;
	pmbutton->oldh = 0;
	pmbutton->mask = NULL;
	pmbutton->policy = GTK_UPDATE_CONTINUOUS;
}

static gboolean gtk_pmbutton_draw(GtkWidget *widget)
{
	GtkPMButton *pmbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMBUTTON (widget), FALSE);
  
	pmbutton = GTK_PMBUTTON (widget);
	if(pmbutton->button[0] == NULL)
	{
		pmbutton->button[0] = gdk_pixmap_new(NULL,widget->allocation.width, widget->allocation.height,24);
		cairo_t *cr = gdk_cairo_create(pmbutton->button[0]);
		cairo_set_source_rgb(cr,0,0,0);
		cairo_rectangle(cr,0,0,widget->allocation.width,widget->allocation.height);
		cairo_fill(cr);
		cairo_save(cr);
		cairo_scale(cr,(widget->allocation.width/143.0),(widget->allocation.height/62.5));
		rsvg_handle_render_cairo(pmbutton->images[0],cr);
		cairo_restore(cr);
		cairo_set_source_rgb(cr,1.0,1.0,1.0);
		cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size (cr,pmbutton->size*(widget->allocation.width/143.5));
		cairo_text_extents_t exts;
		cairo_text_extents(cr,pmbutton->text,&exts);
		cairo_move_to (cr, widget->allocation.width/3+((widget->allocation.width/12)*3)-exts.width/2,widget->allocation.height/2+exts.height/5*2);
		cairo_show_text (cr, pmbutton->text);
		cairo_fill(cr);
		cairo_destroy(cr);
	}
	else
	{
		int width, height;
		gdk_drawable_get_size(pmbutton->button[0],&width, &height);
		if(width!=widget->allocation.width || height!=widget->allocation.height)
		{
			pmbutton->button[0] = gdk_pixmap_new(NULL,widget->allocation.width, widget->allocation.height,24);
			cairo_t *cr = gdk_cairo_create(pmbutton->button[0]);
			cairo_set_source_rgb(cr,0,0,0);
			cairo_rectangle(cr,0,0,widget->allocation.width,widget->allocation.height);
			cairo_fill(cr);
			cairo_save(cr);
			cairo_scale(cr,(widget->allocation.width/143.5),(widget->allocation.height/62.5));
			rsvg_handle_render_cairo(pmbutton->images[0],cr);
			cairo_restore(cr);
			cairo_set_source_rgb(cr,1.0,1.0,1.0);
			cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			cairo_set_font_size (cr,pmbutton->size*(widget->allocation.width/143.5));
			cairo_text_extents_t exts;
			cairo_text_extents(cr,pmbutton->text,&exts);
			cairo_move_to (cr, widget->allocation.width/3+((widget->allocation.width/12)*3)-exts.width/2,widget->allocation.height/2+exts.height/5*2);
			cairo_show_text (cr, pmbutton->text);
			cairo_fill(cr);
			cairo_destroy(cr);
		}
	}
	if(pmbutton->button[1] == NULL)
	{
		pmbutton->button[1] = gdk_pixmap_new(NULL,widget->allocation.width, widget->allocation.height,24);
		cairo_t *cr = gdk_cairo_create(pmbutton->button[1]);
		cairo_set_source_rgb(cr,0,0,0);
		cairo_rectangle(cr,0,0,widget->allocation.width,widget->allocation.height);
		cairo_fill(cr);
		cairo_save(cr);
		cairo_scale(cr,(widget->allocation.width/143.5),(widget->allocation.height/62.5));
		rsvg_handle_render_cairo(pmbutton->images[1],cr);
		cairo_restore(cr);
		cairo_set_source_rgb(cr,1.0,1.0,1.0);
		cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size (cr,pmbutton->size*(widget->allocation.width/143.5));
		cairo_text_extents_t exts;
		cairo_text_extents(cr,pmbutton->text,&exts);
		cairo_move_to (cr, widget->allocation.width/3+((widget->allocation.width/12)*3)-exts.width/2,widget->allocation.height/2+exts.height/5*2);
		cairo_show_text (cr, pmbutton->text);
		cairo_fill(cr);
		cairo_destroy(cr);
	}
	else
	{
		int width, height;
		gdk_drawable_get_size(pmbutton->button[1],&width, &height);
		if(width!=widget->allocation.width || height!=widget->allocation.height)
		{
			pmbutton->button[1] = gdk_pixmap_new(NULL,widget->allocation.width, widget->allocation.height,24);
			cairo_t *cr = gdk_cairo_create(pmbutton->button[1]);
			cairo_set_source_rgb(cr,0,0,0);
			cairo_rectangle(cr,0,0,widget->allocation.width,widget->allocation.height);
			cairo_fill(cr);
			cairo_save(cr);
			cairo_scale(cr,(widget->allocation.width/143.5),(widget->allocation.height/62.5));
			rsvg_handle_render_cairo(pmbutton->images[1],cr);
			cairo_restore(cr);
			cairo_set_source_rgb(cr,1.0,1.0,1.0);
			cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			cairo_set_font_size (cr,pmbutton->size*(widget->allocation.width/143.5));
			cairo_text_extents_t exts;
			cairo_text_extents(cr,pmbutton->text,&exts);
			cairo_move_to (cr, widget->allocation.width/3+((widget->allocation.width/12)*3)-exts.width/2,widget->allocation.height/2+exts.height/5*2);
			cairo_show_text (cr, pmbutton->text);
			cairo_fill(cr);
			cairo_destroy(cr);
		}
	}
	return FALSE;
}

GtkWidget* gtk_pmbutton_new (char *text)
{
	GtkPMButton *pmbutton;

	pmbutton = g_object_new (gtk_pmbutton_get_type (), NULL);
	pmbutton->text = text;
	pmbutton->size = 36-strlen(text)*2;

	return GTK_WIDGET (pmbutton);
}

static void gtk_pmbutton_destroy (GtkObject *object)
{
	GtkPMButton *pmbutton;

	g_return_if_fail (object != NULL);
	g_return_if_fail (GTK_IS_PMBUTTON(object));

	pmbutton = GTK_PMBUTTON(object);


	if (GTK_OBJECT_CLASS (parent_class)->destroy)
		(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}


static void gtk_pmbutton_realize (GtkWidget *widget)
{
	GtkPMButton *pmbutton;
	GdkWindowAttr attributes;
	gint attributes_mask;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (GTK_IS_PMBUTTON(widget));

	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
	pmbutton = GTK_PMBUTTON(widget);

	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.event_mask = gtk_widget_get_events (widget) | 
		GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK |	GDK_BUTTON_RELEASE_MASK;
	attributes.visual = gtk_widget_get_visual (widget);
	attributes.colormap = gtk_widget_get_colormap (widget);

	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
	widget->window = gdk_window_new(widget->parent->window, &attributes, attributes_mask);

	widget->style = gtk_style_attach (widget->style, widget->window);

	gdk_window_set_user_data (widget->window, widget);
	gtk_pmbutton_draw(widget);
	//gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);
}

static void gtk_pmbutton_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
	requisition->width = 143;
	requisition->height = 62;
}

static void gtk_pmbutton_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
	GtkPMButton *pmbutton;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (GTK_IS_PMBUTTON(widget));
	g_return_if_fail (allocation != NULL);

	widget->allocation = *allocation;
	pmbutton = GTK_PMBUTTON(widget);
	widget->allocation.height = widget->allocation.width/2.3;
	if (GTK_WIDGET_REALIZED (widget))
	{
		gdk_window_move_resize (widget->window, allocation->x, allocation->y, allocation->width, allocation->height);

	}
	gtk_pmbutton_draw(widget);
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

	gdk_draw_drawable(widget->window,gdk_gc_new(widget->window),pmbutton->button[pmbutton->state],event->area.x,event->area.y,0,0,event->area.width,event->area.height);
	if(widget->allocation.width!=pmbutton->oldw || widget->allocation.height!=pmbutton->oldh);
	{
		GdkImage *outline = NULL;
		
		outline = gdk_drawable_get_image(widget->window,0,0,widget->allocation.width, widget->allocation.height);
		g_free(pmbutton->mask);
		pmbutton->mask = g_malloc0((outline->width/8)*(outline->height+6));
		int i = 0, j = 0;
		int pix = 0;
		
		for(i=0;i<outline->height;i++)
		{
			for(j = 0;j<outline->width;j+=8)
			{
				int k = 0;
				
				for(k = 0;k<8;k++)
				{
					if((j+k)<outline->width)
					{
						pmbutton->mask[pix] += (unsigned char)(((gdk_image_get_pixel(outline,j+k,i)!=0))<<k);
						//pmbutton->mask[pix] += (unsigned char)(((*(int*)(outline->mem+((j*4)+(k*4)+i*(outline->width)*4)))!=0)<<k);
					}
					else
					{
						break;
					}
				}
				pix++;
			}
		}
		GdkBitmap *mask = gdk_bitmap_create_from_data(NULL,pmbutton->mask,outline->width,outline->height);
		gdk_window_shape_combine_mask(widget->window,mask,0,0);
		pmbutton->oldw = widget->allocation.width;
		pmbutton->oldh = widget->allocation.height;
		
	}
	return FALSE;
}

static gboolean gtk_pmbutton_press(GtkWidget *widget, GdkEventButton *event)
{
	GtkPMButton *pmbutton;
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_PMBUTTON(widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	pmbutton = GTK_PMBUTTON(widget);
 
	if (!pmbutton->state) //KEYWORD - I love dilbert
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
	if (pmbutton->state)
	{
		gtk_grab_remove (widget);
		pmbutton->state = 0; 
		if(event->x > 0 && event->x < widget->allocation.width && event->y > 0 && event->y < widget->allocation.height)
			gtk_signal_emit_by_name(GTK_OBJECT(widget),"clicked");

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
	return FALSE;
}
