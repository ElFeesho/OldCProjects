#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdlib.h>

static GdkColor *banner_create_color(short red, short green, short blue)
{
	GdkColor *nColor = malloc(sizeof(GdkColor));
	nColor->red = red;
	nColor->blue = blue;
	nColor->green = green;
	return nColor;
}

GtkWidget *banner_create(char *text, short size)
{
	GtkWidget *banner_box, *banner_label;

	banner_box = gtk_event_box_new();
	GdkColor *bg = banner_create_color(0,32768,65535);
	gtk_widget_modify_bg(banner_box,GTK_STATE_NORMAL,bg);
	free(bg);
	banner_label = gtk_label_new(text);

	PangoFontDescription *pfd = pango_font_description_new();
	pango_font_description_set_size(pfd,size*1000);
	pango_font_description_set_family(pfd,"Arial");
	pango_font_description_set_style(pfd,PANGO_STYLE_ITALIC);

	GdkColor *col = banner_create_color(65535,65535,65535);
	gtk_widget_modify_font(banner_label,pfd);
	gtk_widget_modify_fg(banner_label,GTK_STATE_NORMAL,col);
	gtk_widget_modify_fg(banner_label,GTK_STATE_ACTIVE,col);
	gtk_widget_modify_fg(banner_label,GTK_STATE_INSENSITIVE,col);
		
	free(col);
	
	gtk_container_add(GTK_CONTAINER(banner_box),banner_label);

	return banner_box;
}
