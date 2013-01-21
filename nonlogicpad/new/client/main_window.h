#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <gtk/gtk.h>

extern void main_window_show();

extern GdkDrawable *main_window_canvas_win;
extern GtkWidget *main_window_topic;
extern void main_window_set_tool(GtkWidget *tool_button, gpointer user_data);

#endif
