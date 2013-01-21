#include <gtk/gtk.h>

static gboolean canvas_mouse_down(GtkWidget *canvas, GdkEventButton *event, gpointer user_data);
static gboolean canvas_mouse_move(GtkWidget *canvas, GdkEventMotion *event, gpointer user_data);
static gboolean canvas_mouse_up(GtkWidget *canvas, GdkEventButton *event, gpointer user_data);
static gboolean canvas_expose(GtkWidget *canvas, GdkEventExpose *event, gpointer user_data);

static double oldx;
static double oldy;
static GdkImage *image;

int main(int argc, char ** argv)
{
	gtk_init(&argc, &argv);
	g_set_application_name("Draw Test");

	GtkWidget *window, *canvas;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	canvas = gtk_drawing_area_new();
	gtk_widget_add_events(canvas,GDK_BUTTON_RELEASE_MASK|GDK_BUTTON_PRESS_MASK|GDK_BUTTON_MOTION_MASK);

	image = gdk_drawable_get_image(GDK_DRAWABLE(canvas->window),0,0,canvas->allocation.width,canvas->allocation.height);
	printf("Hmm\n");
	gtk_container_add(GTK_CONTAINER(window),canvas);

	gtk_widget_show_all(window);
	
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_exit),NULL);	
	g_signal_connect(G_OBJECT(canvas),"button-press-event",G_CALLBACK(canvas_mouse_down),NULL);
	g_signal_connect(G_OBJECT(canvas),"button-release-event",G_CALLBACK(canvas_mouse_up),NULL);
	g_signal_connect(G_OBJECT(canvas),"motion-notify-event",G_CALLBACK(canvas_mouse_move),NULL);
	g_signal_connect(G_OBJECT(canvas),"expose-event",G_CALLBACK(canvas_expose),NULL);
	gtk_main();
	return 0;
}

static gboolean canvas_mouse_down(GtkWidget *canvas, GdkEventButton *event, gpointer user_data)
{
	cairo_t *cr = gdk_cairo_create(canvas->window);
	cairo_move_to(cr,event->x,event->y);
	cairo_line_to(cr,event->x,event->y);
	cairo_stroke(cr);
	cairo_destroy(cr);
	oldx = event->x;
	oldy = event->y;
	return 1;
}

static gboolean canvas_mouse_move(GtkWidget *canvas, GdkEventMotion *event, gpointer user_data)
{
	cairo_t *cr = gdk_cairo_create(canvas->window);
	cairo_move_to(cr,oldx,oldy);
	cairo_line_to(cr,event->x,event->y);
	cairo_stroke(cr);
	cairo_destroy(cr);
	oldx = event->x;
	oldy = event->y;
	
	return 1;
}

static gboolean canvas_mouse_up(GtkWidget *canvas, GdkEventButton *event, gpointer user_data)
{
	image = gdk_drawable_get_image(GDK_DRAWABLE(canvas->window),0,0,canvas->allocation.width,canvas->allocation.height);
	return 1;
}

static gboolean canvas_expose(GtkWidget *canvas, GdkEventExpose *event, gpointer user_data)
{
	if(image!=NULL)
		gdk_draw_image(GDK_DRAWABLE(canvas->window),gdk_gc_new(canvas->window),image,0,0,0,0,canvas->allocation.width,canvas->allocation.height);
	return 1;
}
