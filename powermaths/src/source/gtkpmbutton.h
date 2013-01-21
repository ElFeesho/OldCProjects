#ifndef __GTK_PMBUTTON_H__
#define __GTK_PMBUTTON_H__


#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>


G_BEGIN_DECLS

#define GTK_TYPE_PMBUTTON (gtk_pmbutton_get_type())
#define GTK_PMBUTTON(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_PMBUTTON, GtkPMButton))
#define GTK_PMBUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_PMBUTTON, GtkPMButtonClass))
#define GTK_IS_PMBUTTON(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_PMBUTTON))
#define GTK_IS_PMBUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_PMBUTTON))
#define GTK_PMBUTTON_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_PMBUTTON, GtkPMButtonClass))


typedef struct _GtkPMButton GtkPMButton;
typedef struct _GtkPMButtonClass	GtkPMButtonClass;

struct _GtkPMButton
{
	GtkWidget widget;
	int state;
	char *text;
};

struct _GtkPMButtonClass
{
	GtkWidgetClass parent_class;
	void (*pmbutton_clicked)(GtkWidget *button, gpointer user_data);
	void (*pmbutton_hover)(GtkWidget *button, gpointer user_data);
	void (*pmbutton_leave)(GtkWidget *button, gpointer user_data);
};


GType gtk_pmbutton_get_type(void) G_GNUC_CONST;
GtkWidget* gtk_pmbutton_new(char *label);

G_END_DECLS

#endif /* __GTK_PMBUTTON_H__ */
