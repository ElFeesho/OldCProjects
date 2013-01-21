#ifndef __DRAW_H__
#define __DRAW_H__

#include <gtk/gtk.h>

extern void draw_init_canvas(GtkWidget *canvas);
extern void draw_line(GtkWidget *canvas, int x, int y, int x2, int y2, int r, int g, int b, int width);
extern void draw_border(GtkWidget *canvas);
extern void draw_clear_canvas(GtkWidget *canvas);
extern void draw_sync_buffers(GtkWidget *canvas);
extern void draw_temp_line(GtkWidget *canvas, int x, int y, int x2, int y2, int r, int g, int b, int width);
extern void draw_circle(GtkWidget *canvas, int x, int y, int radius, int r, int g, int b, int width);
extern void draw_temp_circle(GtkWidget *canvas, int x, int y, int radius, int r, int g, int b, int width);
#endif
