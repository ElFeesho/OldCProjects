#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include "question.h"
#include "misc.h"
#include "draw.h"
#include "tool_box.h"
#include "colour_box.h"

static const int WHITEB_FREEHAND	= 1;
static const int WHITEB_LINE		= 2;
static const int WHITEB_ERASER	= 3;
static const int WHITEB_RECT		= 4;
static const int WHITEB_CIRCLE	= 5;

int width = 5;

/*	Callbacks */

static gboolean whiteb_window_delete(GtkWidget *window, gpointer user_data);
static gboolean whiteb_board_press(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data);
static gboolean whiteb_board_motion(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data);
static void whiteb_board_leave(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data);
static gboolean whiteb_board_release(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data);
static gboolean whiteb_expose_event(GtkWidget *board, GdkEventExpose *ev, gpointer user_data);

void whiteb_change_tool(GtkWidget *widget, gpointer user_data);

void whiteb_clear(GtkWidget *canvas, gpointer user_data);

/*	End of Callbacks */

/* Drawing helpers - Shouldn't this be in draw.c? */

static GtkWidget *white_board = NULL;
static GdkPixbuf *whiteb_image = NULL;
static int oldx, oldy;

/* End of drawing helpers */

static int whiteb_tool_mode = 0; 

void whiteb_show(struct question *data)
{
	/* Reset question picture stored in whiteb_image */
	whiteb_image = NULL;
	/* Layout */
	GtkWidget *window;
	GtkWidget *table, *tool_table, *viewport, *header, *colour_table;
		
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"PowerMaths Whiteboard");
	gtk_window_fullscreen(GTK_WINDOW(window));
	
	viewport = gtk_viewport_new(NULL,NULL);
	
	table = gtk_table_new(3,8,0);

	white_board = gtk_drawing_area_new();
	gtk_widget_set_size_request(white_board,768,576);
	draw_init_canvas(white_board);
	gtk_widget_add_events(white_board,GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK|GDK_POINTER_MOTION_MASK|GDK_BUTTON_MOTION_MASK|GDK_LEAVE_NOTIFY_MASK);

	gtk_container_add(GTK_CONTAINER(viewport),white_board);

	tool_table = tool_box_create();
	colour_table = colour_box_create();

	/* Pack */

	gtk_container_add(GTK_CONTAINER(window),table);
	header = misc_create_banner(data->caption,50);
	gtk_widget_set_size_request(header,0,60);
	gtk_table_attach(GTK_TABLE(table),header,0,3,0,1,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),viewport,0,1,1,7,GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,0,0);
	gtk_table_attach(GTK_TABLE(table),tool_table,1,2,1,7,0,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),colour_table,0,2,7,8,GTK_FILL,0,0,0);

	/* Hook up callbacks */
	
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(whiteb_window_delete),NULL);
	
	g_signal_connect(G_OBJECT(white_board),"button-press-event",G_CALLBACK(whiteb_board_press),NULL);
	g_signal_connect(G_OBJECT(white_board),"motion-notify-event",G_CALLBACK(whiteb_board_motion),NULL);
	g_signal_connect(G_OBJECT(white_board),"leave-notify-event",G_CALLBACK(whiteb_board_leave),NULL);
	g_signal_connect(G_OBJECT(white_board),"button-release-event",G_CALLBACK(whiteb_board_release),NULL);
	g_signal_connect(G_OBJECT(white_board),"expose-event",G_CALLBACK(whiteb_expose_event),data);

	gtk_widget_show_all(window);
}

gboolean whiteb_window_delete(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	return 1;
}

gboolean whiteb_board_press(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	oldx = ev->x;
	oldy = ev->y;
	return 1;
}

gboolean whiteb_board_motion(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data)
{
	if(whiteb_tool_mode == WHITEB_LINE)
	{
		if((ev->state & GDK_BUTTON1_MASK)==GDK_BUTTON1_MASK)
		{
			draw_sync_buffers(canvas);
			draw_temp_line(canvas,oldx,oldy,ev->x,ev->y,2);
		}
	}
	else if(whiteb_tool_mode == WHITEB_FREEHAND)
	{
		if((ev->state & GDK_BUTTON1_MASK)==GDK_BUTTON1_MASK)
			draw_line(canvas,oldx,oldy,ev->x, ev->y,0,0,0,2);
		oldx = ev->x;
		oldy = ev->y;
	}
	else if(whiteb_tool_mode == WHITEB_ERASER)
	{
		draw_sync_buffers(canvas);
		draw_temp_rect(canvas,ev->x,ev->y,width);
		if((ev->state & GDK_BUTTON1_MASK)==GDK_BUTTON1_MASK)
			draw_erase(canvas,oldx, oldy,ev->x, ev->y,width);
		oldx = ev->x;
		oldy = ev->y;
	}
	else
	{
		oldx = ev->x;
		oldy = ev->y;
	}
	return 1;
}

gboolean whiteb_board_release(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	if(whiteb_tool_mode == WHITEB_LINE)
	{
		draw_sync_buffers(canvas);
		draw_line(canvas,oldx,oldy,ev->x,ev->y,0,0,0,width);
	}
	if(whiteb_tool_mode == WHITEB_ERASER)
	{
		draw_sync_buffers_update();
	}
	return 1;
}

gboolean whiteb_expose_event(GtkWidget *white_board, GdkEventExpose *ev, gpointer user_data)
{
	struct question *question_data = user_data;
	if(whiteb_image == NULL)
	{
		/* Load the image */
		whiteb_image = rsvg_pixbuf_from_file_at_size(g_strdup_printf("questions/%s/%s/%s/%s/%s/%s",question_data->group,question_data->level,question_data->type,question_data->topic,question_data->stopic,question_data->filename),768,576, NULL);
		if(whiteb_image!=NULL)
		{
			GdkPixmap *question_map = gdk_pixmap_new(NULL,768,576,24);
			draw_fill_a_buffer(question_map);
			gdk_draw_pixbuf(question_map,gdk_gc_new(question_map),whiteb_image,0,0,0,0,768,576,GDK_RGB_DITHER_NONE,0,0);
			draw_set_original(question_map);
		}
	}
	draw_sync_buffers(white_board);
	return 0;
}

void whiteb_board_leave(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data)
{
	draw_sync_buffers(canvas);
}

void whiteb_change_tool(GtkWidget *widget, gpointer user_data)
{
	whiteb_tool_mode = ((int)user_data);
}

void whiteb_clear(GtkWidget *canvas, gpointer user_data)
{
	draw_clear_canvas(white_board);
	draw_sync_buffers(white_board);
}
