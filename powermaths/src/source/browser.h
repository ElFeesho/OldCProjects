#ifndef __BROWSER_H__
#define __BROWSER_H__

#include <stdlib.h>
#include <gtk/gtk.h>

extern void browser_show();
extern void browser_change_viewport(GtkWidget *old_view, GtkWidget *new_view);
extern gboolean browser_viewport_expose(GtkWidget *viewport, GdkEventExpose *ev, gpointer data);
extern GtkWidget *browser_table, *browser_label;
extern void browser_add_history(const char *text, void *(table_create));
void browser_change_label(GtkWidget *new_widget);

#endif
