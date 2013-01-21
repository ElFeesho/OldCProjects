#ifndef __GTK_PMBUTTON_H__
#define __GTK_PMBUTTON_H__


#include <gdk/gdk.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>
#include <librsvg/rsvg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define GTK_PMBUTTON(obj)          GTK_CHECK_CAST (obj, gtk_pmbutton_get_type (), GtkPMButton)
#define GTK_PMBUTTON_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gtk_pmbutton_get_type (), GtkPMButtonClass)
#define GTK_IS_PMBUTTON(obj)       GTK_CHECK_TYPE (obj, gtk_pmbutton_get_type ())


typedef struct _GtkPMButton        GtkPMButton;
typedef struct _GtkPMButtonClass   GtkPMButtonClass;

struct _GtkPMButton
{
	GtkWidget widget;

	guint policy : 2;
	GdkPixmap *button[2];
	char *text;
	double size;
	char *mask;
	int oldw;
	int oldh;
	RsvgHandle *images[2];
	guint8 state;
};

struct _GtkPMButtonClass
{
	GtkWidgetClass parent_class;
	void (*pmbutton_clicked)(GtkWidget *button, gpointer user_data);
	void (*pmbutton_hover)(GtkWidget *button, gpointer user_data);
	void (*pmbutton_leave)(GtkWidget *button, gpointer user_data);
};

GtkWidget* gtk_pmbutton_new(char *text);
GtkType gtk_pmbutton_get_type(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PMBUTTON_H__ */
