#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "draw.h"
#include "globals.h"
#include "con_diag.h"
#include "colour_diag.h"
#include "main_window.h"
#include "about_diag.h"
#include "chan_diag.h"
#include "nick_diag.h"
#include "image_dump.h"
#include "resources.h"

static GtkToolItem *toolbar_conn;
static GtkWidget *toolbar_stroke_width;

/* Callback Declarations */

static gboolean toolbar_stroke_width_changed(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data);

/* End Callbacks */

static void toolbar_init()
{
	if(!toolbar_stroke_width)
	{
		toolbar_stroke_width = gtk_hscale_new_with_range(1,15,1);
		gtk_widget_set_size_request(toolbar_stroke_width,60,30);
		g_signal_connect(G_OBJECT(toolbar_stroke_width),"change-value",G_CALLBACK(toolbar_stroke_width_changed),NULL);
	}
}
void toolbar_switch_conn()
{
	if(!globals_connected)
		gtk_image_set_from_file(GTK_IMAGE(gtk_tool_button_get_icon_widget(GTK_TOOL_BUTTON(toolbar_conn))),resources_file("pixbufs/conn.png"));	
	else
		gtk_image_set_from_file(GTK_IMAGE(gtk_tool_button_get_icon_widget(GTK_TOOL_BUTTON(toolbar_conn))),resources_file("pixbufs/disconn.png"));	
}

GtkWidget *toolbar_get_widget()
{
	toolbar_init();
	GtkTooltips *toolbar_tips = gtk_tooltips_new();
	GtkToolItem *toolbar_item;
	GtkWidget *toolbar = gtk_toolbar_new();
	gtk_toolbar_set_tooltips(GTK_TOOLBAR(toolbar),1);
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar),GTK_TOOLBAR_ICONS);
	
	toolbar_conn = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/conn.png")),NULL);
	gtk_tool_item_set_tooltip(toolbar_conn,toolbar_tips,"Connect/Disconnect from a server",NULL);
	g_signal_connect(G_OBJECT(toolbar_conn),"clicked",G_CALLBACK(con_diag_show),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),toolbar_conn,-1);

	toolbar_item = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/id.png")),NULL);
	gtk_tool_item_set_tooltip(toolbar_item,toolbar_tips,"Change your ID","Your ID is used in the client list and in the text chat");
	g_signal_connect(G_OBJECT(toolbar_item),"clicked",G_CALLBACK(nick_diag_show),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);

	toolbar_item = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/chan.png")),NULL);
	gtk_tool_item_set_tooltip(toolbar_item,toolbar_tips,"Change the channel","The channel you are in affects what content you see");
	g_signal_connect(G_OBJECT(toolbar_item),"clicked",G_CALLBACK(chan_diag_show),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_tool_item_new();
	gtk_container_add(GTK_CONTAINER(toolbar_item),toolbar_stroke_width);
	gtk_tool_item_set_tooltip(toolbar_item,toolbar_tips,"Change the size of your stroke",NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/col.png")),NULL);
	gtk_tool_item_set_tooltip(toolbar_item,toolbar_tips,"Change the colour of your stroke",NULL);
	g_signal_connect(G_OBJECT(toolbar_item),"clicked",G_CALLBACK(colour_diag_show),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
		
	toolbar_item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/brush.png")),NULL);
	gtk_tool_item_set_tooltip(toolbar_item,toolbar_tips,"Freehand Tool",NULL);
	g_signal_connect(G_OBJECT(toolbar_item),"clicked",G_CALLBACK(main_window_set_tool),(int*)0);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/bucket.png")),NULL);
	g_signal_connect(G_OBJECT(toolbar_item),"clicked",G_CALLBACK(main_window_set_tool),(int*)1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/eraser.png")),NULL);
	g_signal_connect(G_OBJECT(toolbar_item),"clicked",G_CALLBACK(main_window_set_tool),(int*)2);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);

	toolbar_item = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/line.png")),NULL);
	gtk_tool_item_set_tooltip(toolbar_item,toolbar_tips,"Line tool",NULL);
	g_signal_connect(G_OBJECT(toolbar_item),"clicked",G_CALLBACK(main_window_set_tool),(int*)3);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);

	toolbar_item = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/rect.png")),NULL);
	gtk_tool_item_set_tooltip(toolbar_item,toolbar_tips,"Rectangle tool",NULL);
	g_signal_connect(G_OBJECT(toolbar_item),"clicked",G_CALLBACK(main_window_set_tool),(int*)4);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);

	toolbar_item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);

	toolbar_item = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/clear.png")),NULL);
	gtk_tool_item_set_tooltip(toolbar_item,toolbar_tips,"Clear the canvas", NULL);
	g_signal_connect(G_OBJECT(toolbar_item),"clicked",G_CALLBACK(draw_clear),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/dump.png")),NULL);
	gtk_tool_item_set_tooltip(toolbar_item,toolbar_tips,"Dump the canvas to a .png",NULL);
	g_signal_connect(G_OBJECT(toolbar_item),"clicked",G_CALLBACK(image_dump),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_tool_button_new(gtk_image_new_from_file(resources_file("pixbufs/about.png")),NULL);
	gtk_tool_item_set_tooltip(toolbar_item,toolbar_tips,"About Nonlogic Pad", NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	g_signal_connect(G_OBJECT(toolbar_item),"clicked",G_CALLBACK(about_diag_show),NULL);
	
	return toolbar;
}

/* Callback Definitions */

gboolean toolbar_stroke_width_changed(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data)
{
	globals_stroke_width = (char)((value>15)?(15):((value<0)?(0):(value)));
	return 0;
}
