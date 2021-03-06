#include <gtk/gtk.h>
#include "menu.h"
#include "toolbar.h"
#include "toolbox.h"

#include "draw.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

/*
 * Static Prototypes
 */

static gboolean window_delete_event(GtkWidget *window, gpointer user_data);

static gboolean window_canvas_down(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data);
static gboolean window_canvas_move(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data);
static gboolean window_canvas_up(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data);
static gboolean window_canvas_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer user_data);

static void window_change_tool(GtkWidget *widget, gpointer tool);

/* End of Prototypes */

/* Globals */
const int TOOL_LINE = 1;
const int TOOL_RECT = 2;
const int TOOL_CIRC = 3;
const int TOOL_TRI = 4;
static int oldx, oldy, toolmode;

/* End Globals */

GtkWidget *window_create()
{
	GtkWidget *window = NULL,
	*main_layout = NULL,
	*menu = NULL,
	*toolbar = NULL,
	*toolbox = NULL,
	*canvas = NULL,
	*question = NULL,
	*answer = NULL;
	
	/*
	 *	Create the main window 
	 */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window),600,570);	
	/*
	 * Create the main table 
	 */
	main_layout = gtk_table_new(6,2,0);
	
	/*
	 * Create the other widgets
	 */
	
	menu = menu_create();
	
	toolbar = toolbar_create(); //Horizontal bar of tools (New, Open etc)
	
	toolbox = toolbox_create(); //Vertical bar of tools (select, rotate scale etc)
	
	canvas = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(canvas),768,576);
	draw_init_canvas(canvas);
	gtk_widget_add_events(canvas, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_BUTTON_MOTION_MASK | GDK_LEAVE_NOTIFY_MASK);
	
	question = gtk_button_new_with_label("Question");
	answer = gtk_button_new_with_label("Answer");
	
	toolmode = TOOL_LINE;
	
	/*
	 * Pack widgets into the table!
	 */
	
	gtk_table_attach(GTK_TABLE(main_layout),menu,0,3,0,1,GTK_FILL|GTK_EXPAND,0,0,0);
	gtk_table_attach(GTK_TABLE(main_layout),toolbar,0,3,1,2,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(main_layout),toolbox,0,1,2,3,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(main_layout),question,0,1,3,4,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(main_layout),answer,0,1,4,5,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(main_layout),canvas,1,2,2,6,0,0,0,0);
	
	gtk_container_add(GTK_CONTAINER(window),main_layout); // Place the main table in with the main window!
	
	/*
	 * Tie up signals
	 */

	g_signal_connect(G_OBJECT(canvas),"expose-event",G_CALLBACK(window_canvas_expose),NULL);
	g_signal_connect(G_OBJECT(canvas),"motion-notify-event",G_CALLBACK(window_canvas_move),NULL);
	g_signal_connect(G_OBJECT(canvas),"button-press-event",G_CALLBACK(window_canvas_down),NULL);
	g_signal_connect(G_OBJECT(canvas),"button-release-event",G_CALLBACK(window_canvas_up),NULL);
	
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(window_delete_event),NULL);
	
	/* Toolbox signals */
	/* Tool box sigs are hooked up here because the meat and potato is here */
	
	g_signal_connect_swapped(G_OBJECT(tb_clear),"clicked",G_CALLBACK(draw_clear_canvas),canvas);
	g_signal_connect(G_OBJECT(tb_line),"clicked",G_CALLBACK(window_change_tool),(void*)TOOL_LINE);	
	g_signal_connect(G_OBJECT(tb_square),"clicked",G_CALLBACK(window_change_tool),(void*)TOOL_RECT);
	g_signal_connect(G_OBJECT(tb_circle),"clicked",G_CALLBACK(window_change_tool),(void*)TOOL_CIRC);
	g_signal_connect(G_OBJECT(tb_triangle),"clicked",G_CALLBACK(window_change_tool),(void*)TOOL_TRI);
	return window;
}

gboolean window_canvas_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer user_data)
{
	draw_border(canvas);
	draw_sync_buffers(canvas);
	return 1;
}

gboolean window_canvas_down(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	switch(toolmode)
	{
		case 1:
		case 2:
		case 3:
			oldx = ev->x;
			oldy = ev->y;
			break;
		default:
			break;
	}
	return 1;
}

gboolean window_canvas_move(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data)
{
	switch(toolmode)
	{
		case 1:
			draw_sync_buffers(canvas);
			draw_temp_line(canvas,oldx,oldy,ev->x, ev->y,0,0,0,2);
			break;
		case 2:
			draw_sync_buffers(canvas);
			draw_temp_line(canvas,oldx,oldy,ev->x, oldy,0,0,0,2);
			draw_temp_line(canvas,ev->x,oldy,ev->x, ev->y,0,0,0,2);
			draw_temp_line(canvas,ev->x,ev->y,oldx, ev->y,0,0,0,2);
			draw_temp_line(canvas,oldx,ev->y,oldx, oldy,0,0,0,2);
			break;
		case 3:
			draw_sync_buffers(canvas);
			draw_temp_circle(canvas,oldx,oldy,max(ev->x,oldx)-min(ev->x,oldx),0,0,0,2);
		default:
			break;
	}
	return 1;
}

gboolean window_canvas_up(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	switch(toolmode)
	{
		case 1:
			draw_sync_buffers(canvas);
			draw_line(canvas,oldx,oldy,ev->x, ev->y,0,0,0,2);
			break;
		case 2:
			draw_sync_buffers(canvas);
			draw_line(canvas,oldx,oldy,ev->x, oldy,0,0,0,2);
			draw_line(canvas,ev->x,oldy,ev->x, ev->y,0,0,0,2);
			draw_line(canvas,ev->x,ev->y,oldx, ev->y,0,0,0,2);
			draw_line(canvas,oldx,ev->y,oldx, oldy,0,0,0,2);
			break;
		case 3:
			draw_sync_buffers(canvas);
			draw_circle(canvas,oldx,oldy,max(ev->x,oldx)-min(ev->x,oldx),0,0,0,2);
			break;
		default:
			break;
	}
	return 1;
}

void window_show()
{
	gtk_widget_show_all(window_create());
}
 
gboolean window_delete_event(GtkWidget *window, gpointer user_data)
{
	gtk_widget_destroy(window);
	gtk_exit(0);
	return 0;
}

void window_change_tool(GtkWidget *widget, gpointer tool)
{
	toolmode = (int)tool;
	printf("Tool mode now : %d\n",toolmode);
}
