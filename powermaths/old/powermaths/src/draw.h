#ifndef __DRAW_H__
#define __DRAW_H__

#include <gtk/gtk.h>
#include "question.h"

extern void draw_thumbnail_question(GtkWidget *widget, struct Question *question_data);
extern void draw_init_canvas(GtkWidget *canvas);
extern void draw_line(GtkWidget *canvas, int x, int y, int x2, int y2, int r, int g, int b, int width);
extern void draw_temp_line(GtkWidget *canvas, int x, int y, int x2, int y2, int r, int g, int b, int width);
extern void draw_fill_buffer();
extern void draw_sync_buffers(GtkWidget *canvas);
extern void draw_sync_buffers_update();
extern void draw_set_original(GdkPixmap *orig);
extern void draw_clear_canvas(GtkWidget *canvas);
extern void draw_fill_a_buffer(GdkDrawable *buffer);
extern void draw_border(GtkWidget *canvas);
extern void draw_erase(GtkWidget *canvas, int x, int y, int x2, int y2, int width);
#endif
