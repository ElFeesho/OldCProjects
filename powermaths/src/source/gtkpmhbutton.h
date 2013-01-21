#ifndef __GTK_PMHBUTTON_H__
#define __GTK_PMHBUTTON_H__


#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>


G_BEGIN_DECLS

#define GTK_TYPE_PMHBUTTON (gtk_pmhbutton_get_type())
#define GTK_PMHBUTTON(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_PMHBUTTON, GtkPMHButton))
#define GTK_PMHBUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_PMHBUTTON, GtkPMHButtonClass))
#define GTK_IS_PMHBUTTON(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_PMHBUTTON))
#define GTK_IS_PMHBUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_PMHBUTTON))
#define GTK_PMHBUTTON_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_PMHBUTTON, GtkPMHButtonClass))


typedef struct _GtkPMHButton GtkPMHButton;
typedef struct _GtkPMHButtonClass	GtkPMHButtonClass;

struct _GtkPMHButton
{
	GtkWidget widget;
	int state;
	char *text;
};

struct _GtkPMHButtonClass
{
	GtkWidgetClass parent_class;
	void (*pmhbutton_clicked)(GtkWidget *button, gpointer user_data);
	void (*pmhbutton_hover)(GtkWidget *button, gpointer user_data);
	void (*pmhbutton_leave)(GtkWidget *button, gpointer user_data);
};


GType gtk_pmhbutton_get_type(void) G_GNUC_CONST;
GtkWidget* gtk_pmhbutton_new	(char *label);

G_END_DECLS

#endif /* __GTK_PMHBUTTON_H__ */
