#ifndef __GTK_VIMAGE_H__
#define __GTK_VIMAGE_H__


#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>
#include "librsvg/rsvg.h"
#include "librsvg/rsvg-cairo.h"


G_BEGIN_DECLS

#define GTK_TYPE_VIMAGE (gtk_vimage_get_type())
#define GTK_VIMAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_VIMAGE, GtkVImage))
#define GTK_VIMAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_VIMAGE, GtkVImageClass))
#define GTK_IS_VIMAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_VIMAGE))
#define GTK_IS_VIMAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_VIMAGE))
#define GTK_VIMAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_VIMAGE, GtkVImageClass))


typedef struct _GtkVImage GtkVImage;
typedef struct _GtkVImageClass	GtkVImageClass;

struct _GtkVImage
{
	GtkWidget widget;
	RsvgHandle *file;
	int width;
	int height;
};

struct _GtkVImageClass
{
	GtkWidgetClass parent_class;
};


GType gtk_vimage_get_type(void) G_GNUC_CONST;
GtkWidget* gtk_vimage_new	(char *file);

G_END_DECLS

#endif /* __GTK_VIMAGE_H__ */
