#include <sys/stat.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "joy.h"

void draw_line(cairo_t *ctx, int x, int y, int x2, int y2, float r, float g, float b)
{
	cairo_set_source_rgb(ctx, r, g, b);
	cairo_move_to(ctx, x, y);
	cairo_line_to(ctx, x2, y2);
	cairo_stroke(ctx);
}

void draw_rect(cairo_t *ctx, int x, int y, int w, int h, float r, float g, float b)
{
	cairo_set_source_rgb(ctx, r, g, b);
	cairo_rectangle(ctx, x, y, w, h);
	cairo_fill_preserve(ctx);
}

gboolean joy_update(GtkWidget *canvas)
{
	static GdkPixmap *bg_buf = NULL;
	static GdkGC *gc = NULL;
	gdk_threads_enter();
	if(bg_buf == NULL)
	{
		bg_buf = gdk_pixmap_new(NULL, canvas->allocation.width, canvas->allocation.height, 24);
		gc = gdk_gc_new(canvas->window);
	}

	int *pos = joy_get_axis();
	int *button = joy_get_button();
	int cx = canvas->allocation.width/2;
	int cy = canvas->allocation.height/2;
	int cw = canvas->allocation.width;
	int ch = canvas->allocation.height;

	cairo_t *cr = gdk_cairo_create(bg_buf);

	// Clear canvas
	draw_rect(cr, 0, 0, cw, ch, 1, 1, 1);

	draw_line(cr, cx, cy, cx + ((pos[0] / 32768.0)*cx), cy + ((pos[1] / 32768.0) * cy), 0, 0, 0);

	draw_line(cr, cx, cy, cx + ((pos[2] / 32768.0)*cx), cy, 1, 0, 0);

	draw_line(cr, cx, cy, cx + ((pos[3] / 32768.0)*cx), cy, 0, 1, 0);

	draw_line(cr, cx, cy, cx + ((pos[4] / 32768.0)*cx), cy + ((pos[5] / 32768.0) * cy), 0, 0, 1);

	int i = 0;
	for(; i<128; i++)
	{
		if(button[i])
		{
			// High value represents button press
			draw_rect(cr, i*20, (i/10)*10, 10, 10, 0,0,0);
		}
	}

	cairo_destroy(cr);
	gdk_draw_drawable(canvas->window, gc, bg_buf, 0, 0, 0, 0, cw, ch);
	gdk_threads_leave();
	return TRUE;
}

int main(int argc, char ** argv)
{
	GtkWidget *window, *canvas;
	gtk_init(&argc, &argv);

	char *joydev = NULL;

	if(argc == 1)
	{
		joydev = "/dev/input/js0";
		struct stat joystat;
		if(stat(joydev, &joystat)<0)
		{
			fprintf(stderr, "Pass a joystick device\n");
			return -1;
		}
	}
	else
	{
		joydev = argv[1];
	}

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	canvas = gtk_drawing_area_new();

	gtk_container_add(GTK_CONTAINER(window), canvas);

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_exit),NULL);

	joy_open(joydev);

	g_timeout_add(1000/60,(GSourceFunc)joy_update,canvas);
	g_thread_init(NULL);
	g_thread_create((GThreadFunc)joy_poll,NULL,FALSE,NULL);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
