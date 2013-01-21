#ifndef __MISC_H__
#define __MISC_H__

#include <gtk/gtk.h>

extern GdkColor *misc_create_color(short red, short green, short blue);
extern GtkWidget *misc_create_banner(char *text, unsigned short size);
extern void misc_set_background(GtkWidget *widget, guint16 r, guint16 g, guint16 b);
extern char **misc_tokenize_string(const char *string, char delim, int *count);

#endif
