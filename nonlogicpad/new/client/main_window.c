#include <gtk/gtk.h>
#include <string.h>
#include "string_ops.h"
#include "chat.h"
#include "globals.h"
#include "draw.h"
#include "client.h"
#include "network.h"
#include "toolbar.h"
#include "colour_widget.h"
#include "config.h" //Poor poor autotools
#include "resources.h"

/* Callbacks */

static gboolean main_window_delete_event(GtkWidget *window, gpointer user_data);
static gboolean main_window_message_activate(GtkWidget *entry, gpointer user_data);
static gboolean main_window_canvas_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer user_data);
static gboolean main_window_canvas_mouse_down(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data);
static gboolean main_window_canvas_mouse_move(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data);
static gboolean main_window_canvas_mouse_up(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data);
static void main_window_topic_activate(GtkWidget *topic, gpointer user_data);

/* End of callbacks */

static int main_window_oldx, main_window_oldy, main_window_tool_mode;
static GdkCursor *main_window_cursors[3];

GdkDrawable *main_window_canvas_win = NULL;
GtkWidget *main_window_topic = NULL;

void main_window_show()
{
	GtkWidget *main_window, *main_window_canvas, *main_window_message;

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(main_window,660,660);

	main_window_canvas = gtk_drawing_area_new();
	gtk_widget_set_size_request(main_window_canvas,500,500);

	main_window_message = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(main_window_message),"");
	gtk_entry_set_max_length(GTK_ENTRY(main_window_message),255);

	main_window_topic = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(main_window_topic),"");
	gtk_entry_set_max_length(GTK_ENTRY(main_window_topic),128);

	GtkWidget *main_window_vbox3 = gtk_vbox_new(0,0); // contains EVERYTHING
	GtkWidget *main_window_vbox = gtk_vpaned_new(); // Holds [canvas userlist]/[history messagebox]
	GtkWidget *main_window_hbox = gtk_hpaned_new(); // Holds the canvas and user list
	GtkWidget *main_window_vbox2 = gtk_vbox_new(0,0);
	GtkWidget *main_window_hbox2 = gtk_hbox_new(0,0);
	GtkWidget *main_window_hbox3 = gtk_hbox_new(0,0);
	
	gtk_box_pack_start(GTK_BOX(main_window_hbox3),gtk_label_new("Topic:"),0,0,0);
	gtk_box_pack_start(GTK_BOX(main_window_hbox3),main_window_topic,GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,0);
	gtk_box_pack_start(GTK_BOX(main_window_vbox3),toolbar_get_widget(),0,0,0);
	gtk_box_pack_start(GTK_BOX(main_window_vbox3),main_window_hbox3,0,0,0);
	gtk_box_pack_start(GTK_BOX(main_window_vbox3),main_window_vbox,1,1,0);
	gtk_box_pack_start(GTK_BOX(main_window_vbox2),chat_get_history_widget(),1,1,0);
	gtk_box_pack_start(GTK_BOX(main_window_vbox2),main_window_message,0,1,0);
	gtk_box_pack_start(GTK_BOX(main_window_hbox2),main_window_vbox2,1,1,0);
	gtk_box_pack_start(GTK_BOX(main_window_hbox2),colour_widget_get_widget(),0,0,0);
	gtk_paned_add1(GTK_PANED(main_window_hbox),main_window_canvas);
	gtk_paned_pack2(GTK_PANED(main_window_hbox),client_get_widget(),0,0);
	gtk_paned_add1(GTK_PANED(main_window_vbox),main_window_hbox);
	gtk_paned_add2(GTK_PANED(main_window_vbox),main_window_hbox2);
	gtk_container_add(GTK_CONTAINER(main_window),main_window_vbox3);

	gtk_widget_set_events(main_window_canvas, gtk_widget_get_events(main_window_canvas) | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_BUTTON_MOTION_MASK | GDK_POINTER_MOTION_MASK | GDK_LEAVE_NOTIFY_MASK);

	/* Get the config for NLPad - doi! Config contains nickname, last channel */
	config_load();

	/*
		Load the cursors for use later on the drawing area 
		- brush is default tool 
	*/
	main_window_tool_mode = 0;
	main_window_cursors[0] = gdk_cursor_new_from_pixbuf(gdk_display_get_default(),gdk_pixbuf_new_from_file(resources_file("pixbufs/brush.png"),NULL),2,18);
	main_window_cursors[1] = gdk_cursor_new_from_pixbuf(gdk_display_get_default(),gdk_pixbuf_new_from_file(resources_file("pixbufs/bucket.png"),NULL),2,18);
	main_window_cursors[2] = gdk_cursor_new_from_pixbuf(gdk_display_get_default(),gdk_pixbuf_new_from_file(resources_file("pixbufs/eraser.png"),NULL),2,18);
	/* 
		Initialise globals - Such as nickname, channel and so on incase 
		no config was found or parts were missing.
	*/

	globals_init();

	/* Now that all globals should have been loaded- add the user into the client list */

	client_add(globals_nick,-1); /* The user always has an ID of -1 so they are at the top of the list */
	
	/* Add a welcome message to the chat box	*/
	chat_append("Welcome to Nonlogic Pad. Type /list for a list of commands!\n");

	/* Thrust our trusty window on screen */
	gtk_widget_show_all(main_window);

	/* Apply the cursor we made earlier */
	gdk_window_set_cursor(main_window_canvas->window,main_window_cursors[0]);

	/* Set the canvas_win pointer to the canvas's window */
	main_window_canvas_win = main_window_canvas->window;

	/* Show some instructions on the canvas */
	draw_welcome();

	/* Hookup Callbacks */

	g_signal_connect(G_OBJECT(main_window),"delete-event",G_CALLBACK(main_window_delete_event),NULL);
	g_signal_connect(G_OBJECT(main_window_message),"activate",G_CALLBACK(main_window_message_activate),NULL);
	g_signal_connect(G_OBJECT(main_window_canvas),"button-press-event",G_CALLBACK(main_window_canvas_mouse_down),NULL);
	g_signal_connect(G_OBJECT(main_window_canvas),"motion-notify-event",G_CALLBACK(main_window_canvas_mouse_move),NULL);
	g_signal_connect(G_OBJECT(main_window_canvas),"button-release-event",G_CALLBACK(main_window_canvas_mouse_up),NULL);
	g_signal_connect(G_OBJECT(main_window_canvas),"expose-event",G_CALLBACK(main_window_canvas_expose),NULL);
	g_signal_connect(G_OBJECT(main_window_canvas),"leave-notify-event",G_CALLBACK(main_window_canvas_expose),NULL);
	g_signal_connect(G_OBJECT(main_window_topic),"activate",G_CALLBACK(main_window_topic_activate),NULL);
}

void main_window_set_tool(GtkWidget *tool_button, gpointer user_data)
{
	main_window_tool_mode = (int)user_data;
	if(main_window_tool_mode <=2)
		gdk_window_set_cursor(main_window_canvas_win,main_window_cursors[main_window_tool_mode]);
	else if(main_window_tool_mode > 2)
		gdk_window_set_cursor(main_window_canvas_win,gdk_cursor_new(GDK_CROSSHAIR)); /* Other tools use cross hair */	
}

/* Definition of Callbacks */

gboolean main_window_delete_event(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	network_disconnect(-1);
	gtk_exit(0);
	return 1;
}

gboolean main_window_message_activate(GtkWidget *entry, gpointer user_data)
{
	const char *msg = gtk_entry_get_text(GTK_ENTRY(entry));
	if(strlen(msg)==0)
		return 0;

	if(msg[0]=='/' && strlen(msg)>1)
	{
		if(strncmp(sub_str((char*)msg,' ',0,5),"/nick",5)==0) /* Nick change */
		{
			if(sub_str((char*)msg,' ',1,16)!=NULL) // Catch more segs
			{
				strncpy(globals_nick,sub_str((char*)msg,' ',1,16),16);
				client_change(globals_nick,-1);
				network_send_nick();
			}
		}
		else if(strncmp(sub_str((char*)msg,' ',0,3),"/me",3)==0)
		{
			chat_add_action(-1,(char*)(msg+4),strlen(msg+4));
			network_send_action((char*)(msg+4));
		}
		else if(strncmp(sub_str((char*)msg,' ',0,5),"/list",5)==0)
		{
			chat_append("List of Commands:\n\t/nick [new name]\n\t/me [some action]\n");
		}
		gtk_entry_set_text(GTK_ENTRY(entry),"");
		return 0;
	}
	chat_add_msg(-1,(char*)msg,strlen(msg));
	network_send_message((char*)msg);
	gtk_entry_set_text(GTK_ENTRY(entry),"");	
	return 1;
}

gboolean main_window_canvas_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer user_data)
{
	draw_sync_buffers();
	return 1;
}

gboolean main_window_canvas_mouse_down(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	if(ev->button == 1)
	{
		main_window_oldx = ev->x;
		main_window_oldy = ev->y;
		
		if(main_window_tool_mode == 0) // Brush
		{
			draw_sync_buffers();
			draw_line(ev->x,ev->y,ev->x,ev->y, globals_red, globals_green, globals_blue, globals_stroke_width);
			network_send_line(ev->x,ev->y,ev->x,ev->y);
		}
		if(main_window_tool_mode == 1) // Bucket
		{
			draw_flood_fill(ev->x, ev->y, draw_16_to_8(globals_red), draw_16_to_8(globals_green), draw_16_to_8(globals_blue));
			network_send_fill(ev->x, ev->y);
		}
		if(main_window_tool_mode == 2) // Eraser
		{
			draw_sync_buffers();
			draw_line(ev->x,ev->y,ev->x,ev->y, 65535, 65535, 65535, globals_stroke_width);
			network_send_erase(ev->x,ev->y,ev->x,ev->y);
		}
	}
	return 0;
}

gboolean main_window_canvas_mouse_move(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data)
{
	if((ev->state & GDK_BUTTON1_MASK) == GDK_BUTTON1_MASK)
	{
		if(main_window_tool_mode == 0)
		{
			draw_line(main_window_oldx,main_window_oldy,ev->x,ev->y, globals_red, globals_green, globals_blue, globals_stroke_width);
			network_send_line(main_window_oldx,main_window_oldy,ev->x,ev->y);
			main_window_oldx = ev->x;
			main_window_oldy = ev->y;
		}
		if(main_window_tool_mode == 2)
		{
			draw_line(main_window_oldx,main_window_oldy,ev->x,ev->y, 65535, 65535, 65535, globals_stroke_width);
			main_window_oldx = ev->x;
			main_window_oldy = ev->y;
			network_send_erase(main_window_oldx,main_window_oldy,ev->x,ev->y);
		}
		if(main_window_tool_mode == 3) // Line
		{
			draw_sync_buffers();
			if((ev->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK)
			{
				draw_temp_line(main_window_oldx, main_window_oldy, ev->x, main_window_oldy+(ev->x-main_window_oldx),0,0,0,1);
			}
			else
				draw_temp_line(main_window_oldx, main_window_oldy, ev->x, ev->y,0,0,0,1);
		}
		if(main_window_tool_mode == 4) // Rect
		{
			draw_sync_buffers();
			draw_temp_rect(main_window_oldx, main_window_oldy, ev->x, ev->y,0,0,0,1);
		}
	}
	else
	{
		if(main_window_tool_mode == 0 || main_window_tool_mode == 2)
		{
			draw_sync_buffers();
			draw_temp_outline(ev->x,ev->y,globals_stroke_width);
		}
	}
	return 0;
}

gboolean main_window_canvas_mouse_up(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{

	if(ev->button == 1)
	{
		if(main_window_tool_mode == 3) // Line
		{
			draw_line(main_window_oldx,main_window_oldy,ev->x,ev->y, globals_red, globals_green, globals_blue, globals_stroke_width);
			network_send_line(main_window_oldx,main_window_oldy,ev->x,ev->y);
		}
		if(main_window_tool_mode == 4) // Rect
		{
			draw_rect(main_window_oldx,main_window_oldy,ev->x,ev->y, globals_red, globals_green, globals_blue, globals_stroke_width);
			network_send_rect(main_window_oldx,main_window_oldy,ev->x,ev->y);
		}
	}
	return 0;
}

static void main_window_topic_activate(GtkWidget *topic, gpointer user_data)
{
	if(globals_connected == 1)
	{
		network_send_topic((char*)gtk_entry_get_text(GTK_ENTRY(topic)));
	}
}

/* End of Definitions of Callbacks */
