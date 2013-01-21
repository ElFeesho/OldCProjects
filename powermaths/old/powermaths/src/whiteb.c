#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include "question.h"
#include "misc.h"
#include "draw.h"

const int WHITEB_FREEHAND =	1;
const int WHITEB_LINE =		2;
const int WHITEB_ERASER =	3;
const int WHITEB_RECT 	=	4;
const int WHITEB_CIRCLE =	5;

/*	Callbacks */

static gboolean whiteb_window_delete(GtkWidget *window, gpointer user_data);
static gboolean whiteb_board_press(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data);
static gboolean whiteb_board_motion(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data);
static gboolean whiteb_board_release(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data);
static gboolean whiteb_expose_event(GtkWidget *board, GdkEventExpose *ev, gpointer user_data);

static void whiteb_change_tool(GtkWidget *widget, gpointer user_data);

static void whiteb_clear(GtkWidget *canvas, gpointer user_data);

/*	End of Callbacks */

/* Drawing helpers - Shouldn't this be in draw.c? */

static GtkWidget *white_board = NULL;
static GdkPixbuf *whiteb_image = NULL;
static int oldx, oldy;

/* End of drawing helpers */

static int whiteb_tool_mode = 0; 

void whiteb_show(struct Question *data)
{
	/* Reset question picture stored in whiteb_image */
	whiteb_image = NULL;
	/* Layout */
	GtkWidget *window;
	GtkWidget *table, *tool_table, *viewport;

	/* UI */
	GtkWidget *clear_board = NULL,
				*freeh_tool = NULL,
				*line_tool = NULL,
				*rect_tool = NULL,
				*circ_tool = NULL,
				*erase_tool = NULL,
				*exit_but = NULL;
		
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"Powermaths Whiteboard");
	gtk_window_fullscreen(GTK_WINDOW(window));
	
	viewport = gtk_viewport_new(NULL,NULL);
	
	table = gtk_table_new(3,8,0);

	white_board = gtk_drawing_area_new();
	gtk_widget_set_size_request(white_board,768,576);
	draw_init_canvas(white_board);
	gtk_widget_modify_bg(white_board,GTK_STATE_NORMAL,misc_create_color(65535,65535,65535));
	gtk_widget_add_events(white_board,GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK|GDK_BUTTON_MOTION_MASK);

	gtk_container_add(GTK_CONTAINER(viewport),white_board);

	tool_table = gtk_table_new(1,8,1);

	clear_board = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(clear_board),gtk_image_new_from_file("pixmaps/clear.png"));
	
	freeh_tool = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(freeh_tool),gtk_image_new_from_file("pixmaps/freeh.png"));
	
	line_tool = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(line_tool),gtk_image_new_from_file("pixmaps/line.png"));
	
	rect_tool = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(rect_tool),gtk_image_new_from_file("pixmaps/square.png"));	
	
	circ_tool = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(circ_tool),gtk_image_new_from_file("pixmaps/circle.png"));	
	
	erase_tool = gtk_button_new();	
	gtk_container_add(GTK_CONTAINER(erase_tool),gtk_image_new_from_file("pixmaps/eraser.png"));	

	exit_but = gtk_button_new_with_label("Exit");

	/* Pack */

	gtk_container_add(GTK_CONTAINER(window),table);
	
	gtk_table_attach(GTK_TABLE(table),misc_create_banner(data->caption,16000),0,3,0,1,GTK_FILL|GTK_EXPAND,0,0,0);
	gtk_table_attach(GTK_TABLE(table),viewport,0,1,1,8,0,0,0,0);
	gtk_table_attach(GTK_TABLE(table),tool_table,1,2,1,8,0,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(tool_table),clear_board,0,1,1,2,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_table),freeh_tool,0,1,2,3,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_table),line_tool,0,1,3,4,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_table),rect_tool,0,1,4,5,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_table),circ_tool,0,1,5,6,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_table),erase_tool,0,1,6,7,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(tool_table),exit_but,0,1,7,8,GTK_FILL,0,0,0);
	

	/* Hook up callbacks */
	
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(whiteb_window_delete),NULL);
	
	g_signal_connect(G_OBJECT(white_board),"button-press-event",G_CALLBACK(whiteb_board_press),NULL);
	g_signal_connect(G_OBJECT(white_board),"motion-notify-event",G_CALLBACK(whiteb_board_motion),NULL);
	g_signal_connect(G_OBJECT(white_board),"button-release-event",G_CALLBACK(whiteb_board_release),NULL);
	g_signal_connect(G_OBJECT(white_board),"expose-event",G_CALLBACK(whiteb_expose_event),data);
	g_signal_connect(G_OBJECT(clear_board),"clicked",G_CALLBACK(whiteb_clear),NULL);
	g_signal_connect(G_OBJECT(freeh_tool),"clicked",G_CALLBACK(whiteb_change_tool),(void*)WHITEB_FREEHAND);
	g_signal_connect(G_OBJECT(line_tool),"clicked",G_CALLBACK(whiteb_change_tool),(void*)WHITEB_LINE);
	g_signal_connect(G_OBJECT(rect_tool),"clicked",G_CALLBACK(whiteb_change_tool),(void*)WHITEB_RECT);
	g_signal_connect(G_OBJECT(circ_tool),"clicked",G_CALLBACK(whiteb_change_tool),(void*)WHITEB_CIRCLE);
	g_signal_connect(G_OBJECT(erase_tool),"clicked",G_CALLBACK(whiteb_change_tool),(void*)WHITEB_ERASER);
	
	g_signal_connect_swapped(G_OBJECT(exit_but),"clicked",G_CALLBACK(whiteb_window_delete),window);

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
		draw_sync_buffers(canvas);
		draw_temp_line(canvas,oldx,oldy,ev->x,ev->y,0,0,0,2);
	}
	else if(whiteb_tool_mode == WHITEB_FREEHAND)
	{
		draw_line(canvas,oldx,oldy,ev->x, ev->y,0,0,0,2);
		oldx = ev->x;
		oldy = ev->y;
	}
	else if(whiteb_tool_mode == WHITEB_ERASER)
	{
		draw_erase(canvas,oldx,oldy,ev->x, ev->y,2);
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
		draw_line(canvas,oldx,oldy,ev->x,ev->y,0,0,0,2);
	}
	if(whiteb_tool_mode == WHITEB_ERASER)
	{
		draw_sync_buffers_update();
	}
	return 1;
}

gboolean whiteb_expose_event(GtkWidget *white_board, GdkEventExpose *ev, gpointer user_data)
{
	struct Question *data = user_data;
	if(whiteb_image == NULL)
	{
		/* Load the image */
		char *filename = malloc(140);
		memset(filename,'\0',140);
		strcpy(filename,"questions/");
		strcat(filename,data->filename);
		whiteb_image = rsvg_pixbuf_from_file_at_size(filename,768,576, NULL);
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

void whiteb_change_tool(GtkWidget *widget, gpointer user_data)
{
	whiteb_tool_mode = ((int)user_data);
}

void whiteb_clear(GtkWidget *canvas, gpointer user_data)
{
	draw_clear_canvas(white_board);
	draw_sync_buffers(white_board);
}
