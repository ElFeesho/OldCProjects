#ifndef __DRAW_H__
#define __DRAW_H__

#include <gdk/gdk.h>

extern GdkColor *draw_color_create(gint16 red, gint16 green, gint16 blue);
extern void draw_line(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width);
extern void draw_rect(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width);
extern void draw_circle(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width);
extern void draw_temp_line(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width);
extern void draw_temp_rect(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width);
extern void draw_temp_circle(int x, int y, int x2, int y2, gint16 r, gint16 g, gint16 b, int width);
extern void draw_temp_outline(int x, int y, int stroke);
extern void draw_welcome();
extern void draw_resize_buffer(int w, int h);
extern void draw_clear();
extern void draw_clear_buffers();
extern void draw_sync_buffers(GdkEventExpose *ev);
extern void draw_flood_fill(int x, int y, guint8 r, guint8 g, guint8 b);
extern GdkPixmap *draw_get_backbuffer();
extern unsigned char draw_16_to_8(guint16 value);
#endif
