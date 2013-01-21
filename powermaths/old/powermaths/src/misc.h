#ifndef __MISC_H__
#define __MISC_H__

#include <gtk/gtk.h>

extern GdkColor *misc_create_color(short red, short green, short blue);
extern GtkWidget *misc_create_banner(char *text, unsigned short size);

#endif
