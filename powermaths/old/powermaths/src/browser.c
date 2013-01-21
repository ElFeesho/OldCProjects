/*
 * 	First real window that is seen!
 */
#include <stdlib.h>
#include "database.h"
#include "board.h"
#include "misc.h"
#include "levels.h"
#include "groups.h"

/* Callbacks */

static gboolean browser_window_close(GtkWidget *window, gpointer user_data);
static gboolean browser_scroller_expose(GtkWidget *scroller, GdkEventExpose *ev, gpointer data);
static void browser_home_clicked(GtkWidget *home, gpointer data);
gboolean browser_viewport_expose(GtkWidget *viewport, GdkEventExpose *ev, gpointer data);
/* End of Callbacks */

static GtkWidget *browser_viewport = NULL, *browser_current_view = NULL;

void browser_change_viewport(GtkWidget *old_view, GtkWidget *new_view)
{
	gtk_container_remove(GTK_CONTAINER(browser_viewport),browser_current_view);
	gtk_container_add(GTK_CONTAINER(browser_viewport), new_view);
	gtk_widget_show_all(new_view);
	gtk_widget_show(browser_viewport);
	browser_current_view = new_view;
}

void browser_show()
{
	/* Main Window */
	GtkWidget *window, *vbox, *scroller, *scroller_vp, *home, *exit_button;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"Powermaths");	
	gtk_window_fullscreen(GTK_WINDOW(window));
	
	scroller = gtk_hbox_new(0,5);
	scroller_vp = gtk_viewport_new(NULL,NULL);
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(scroller_vp),GTK_SHADOW_NONE);
	gtk_container_add(GTK_CONTAINER(scroller_vp),scroller);
	
	browser_viewport = gtk_viewport_new(NULL,NULL);
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(browser_viewport),GTK_SHADOW_NONE);
	
	home = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(home),gtk_image_new_from_file("pixmaps/home.png"));
	
	exit_button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(exit_button),gtk_image_new_from_file("pixmaps/exit.png"));
	
	gtk_box_pack_start(GTK_BOX(scroller),home,0,0,0);
	gtk_box_pack_end(GTK_BOX(scroller),exit_button,0,0,0);
	
	vbox = gtk_vbox_new(0,0);
	browser_current_view = groups_create_viewport();
	gtk_container_add(GTK_CONTAINER(browser_viewport),browser_current_view);
	gtk_box_pack_start(GTK_BOX(vbox),browser_viewport,1,1,0);
	gtk_box_pack_start(GTK_BOX(vbox),scroller_vp,0,0,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	g_signal_connect(G_OBJECT(browser_viewport),"expose-event",G_CALLBACK(browser_viewport_expose),NULL);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(browser_window_close),NULL);	
	g_signal_connect(G_OBJECT(home),"clicked",G_CALLBACK(browser_home_clicked),NULL);
	g_signal_connect(G_OBJECT(exit_button),"clicked",G_CALLBACK(gtk_exit),NULL);
	g_signal_connect(G_OBJECT(scroller),"expose-event",G_CALLBACK(browser_scroller_expose),NULL);
	gtk_widget_show_all(window);
}

static gboolean browser_window_close(GtkWidget *window, gpointer user_data)
{
	gtk_exit(0);
	return 0;
}

gboolean browser_viewport_expose(GtkWidget *viewport, GdkEventExpose *ev, gpointer data)
{
	static GdkPixmap *bg = NULL;
	if(bg == NULL)
	{
		bg = gdk_pixmap_new(NULL,viewport->allocation.width, viewport->allocation.height,24);
		cairo_t *cr = gdk_cairo_create(bg);
		cairo_rectangle(cr,0,0,viewport->allocation.width,viewport->allocation.height);
		cairo_pattern_t* gradient = cairo_pattern_create_linear(0,0,viewport->allocation.width, viewport->allocation.height);
		cairo_pattern_add_color_stop_rgb(gradient, 0.0,0.4,0.4,0.4);
		cairo_pattern_add_color_stop_rgb(gradient, 0.2,0.8,0.8,0.8);
		cairo_pattern_add_color_stop_rgb(gradient, 0.6,0.4,0.4,0.4);
		cairo_pattern_add_color_stop_rgb(gradient, 1.0,0.1,0.1,0.1);
		cairo_set_source(cr,gradient);
		cairo_fill(cr);
		cairo_destroy(cr);
	}
	gdk_draw_drawable(viewport->window,gdk_gc_new(viewport->window),bg,0,0,0,0,viewport->allocation.width, viewport->allocation.height);
	return 0;
}

gboolean browser_scroller_expose(GtkWidget *scroller, GdkEventExpose *ev, gpointer data)
{
	static GdkPixmap *bg = NULL;
	if(bg == NULL)
	{
		bg = gdk_pixmap_new(NULL,scroller->allocation.width, scroller->allocation.height,24);
		cairo_t *cr = gdk_cairo_create(bg);
		cairo_rectangle(cr,0,0,scroller->allocation.width,scroller->allocation.height);
		cairo_pattern_t* gradient = cairo_pattern_create_linear(0,0,0, scroller->allocation.height);
		cairo_pattern_add_color_stop_rgb(gradient, 0.0,0.4,0.4,0.4);
		cairo_pattern_add_color_stop_rgb(gradient, 0.2,0.8,0.8,0.8);
		cairo_pattern_add_color_stop_rgb(gradient, 0.6,0.4,0.4,0.4);
		cairo_pattern_add_color_stop_rgb(gradient, 1.0,0.1,0.1,0.1);
		cairo_set_source(cr,gradient);
		cairo_fill(cr);
		cairo_destroy(cr);
	}
	gdk_draw_drawable(scroller->window,gdk_gc_new(scroller->window),bg,0,0,0,0,scroller->allocation.width, scroller->allocation.height);
	return 0;
}

void browser_home_clicked(GtkWidget *home, gpointer data)
{
	browser_change_viewport(browser_current_view,groups_create_viewport());
}
