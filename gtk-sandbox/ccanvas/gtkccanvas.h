#ifndef __CCANVAS_H__
#define __CCANVAS_H__

#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

#define GTK_TYPE_CCANVAS (gtk_ccanvas_get_type())
#define GTK_CCANVAS(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_CCANVAS, GtkCCanvas))
#define GTK_CCANVAS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_CCANVAS, GtkCCanvasClass))
#define GTK_IS_CCANVAS(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_CCANVAS))
#define GTK_IS_CCANVAS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_CCANVAS))
#define GTK_CCANVAS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_CCANVAS, GtkCCanvasClass))


typedef struct _GtkCCanvas GtkCCanvas;
typedef struct _GtkCCanvasClass	GtkCCanvasClass;

typedef enum _GtkCCanvasTool { GTK_CCANVAS_FREEH, GTK_CCANVAS_LINE, GTK_CCANVAS_RECT } GtkCCanvasTool;

struct _GtkCCanvas
{
	GtkWidget widget;
	int border : 1;
	int show_outline : 1;
	int incanv : 1;
	int mdown : 1;
	int toolmode : 8;
	unsigned int brush_size;
	int ox,oy,mx,my;
	GdkPixmap *back_buffer;
};

struct _GtkCCanvasClass
{
	GtkWidgetClass parent_class;
};


GType gtk_ccanvas_get_type(void) G_GNUC_CONST;
GtkWidget* gtk_ccanvas_new();
GtkWidget* gtk_ccanvas_new_with_border();

gboolean gtk_ccanvas_set_border(GtkCCanvas *ccanvas, gboolean border);
gboolean gtk_ccanvas_get_border(GtkCCanvas *ccanvas);

gboolean gtk_ccanvas_set_outline(GtkCCanvas *ccanvas, gboolean outline);
gboolean gtk_ccanvas_get_outline(GtkCCanvas *ccanvas);

gboolean gtk_ccanvas_set_brush_size(GtkCCanvas *ccanvas, guint size);
gint gtk_ccanvas_get_brush_size(GtkCCanvas *ccanvas);

void gtk_ccanvas_set_tool(GtkCCanvas *ccanvas, GtkCCanvasTool tool);

G_END_DECLS

#endif
