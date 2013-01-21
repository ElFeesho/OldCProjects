#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GdkColor *misc_create_color(short red, short green, short blue)
{
	GdkColor *nColor = malloc(sizeof(GdkColor));
	nColor->red = red;
	nColor->blue = blue;
	nColor->green = green;
	return nColor;
}

typedef struct banner_info
{
	short size;
	char *text;
} banner_info;

static gboolean banner_box_expose(GtkWidget *banner, GdkEventExpose *ev, banner_info *data)
{
	cairo_t *cr = gdk_cairo_create(banner->window);
	
	cairo_set_source_rgb(cr,1.0,1.0,1.0);
	cairo_select_font_face (cr, "Arial", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, data->size);
	cairo_text_extents_t exts;
	cairo_text_extents(cr,data->text,&exts);
	cairo_move_to (cr, banner->allocation.width/2-exts.width/2,banner->allocation.height/2+exts.height/3);
	cairo_show_text (cr, data->text);
	cairo_fill(cr);
	
	cairo_destroy(cr);
	return 1;
}

void misc_set_background(GtkWidget *widget, guint16 r, guint16 g, guint16 b)
{
	gtk_widget_modify_base(widget,GTK_STATE_ACTIVE,misc_create_color(r,g,b));
	gtk_widget_modify_base(widget,GTK_STATE_NORMAL,misc_create_color(r,g,b));
	gtk_widget_modify_base(widget,GTK_STATE_SELECTED,misc_create_color(r,g,b));
	gtk_widget_modify_base(widget,GTK_STATE_INSENSITIVE,misc_create_color(r,g,b));
	gtk_widget_modify_bg(widget,GTK_STATE_ACTIVE,misc_create_color(r,g,b));	
	gtk_widget_modify_bg(widget,GTK_STATE_NORMAL,misc_create_color(r,g,b));	
	gtk_widget_modify_bg(widget,GTK_STATE_SELECTED,misc_create_color(r,g,b));	
	gtk_widget_modify_bg(widget,GTK_STATE_INSENSITIVE,misc_create_color(r,g,b));	
}

GtkWidget *misc_create_banner(char *text, unsigned short size)
{
	
	GtkWidget *banner_box = NULL;

	banner_box = gtk_drawing_area_new();
	misc_set_background(banner_box,42405,0,8481);
	banner_info *new_info = g_malloc0(sizeof(banner_info));
	new_info->size = size;
	new_info->text = text;
	g_signal_connect(G_OBJECT(banner_box),"expose-event",G_CALLBACK(banner_box_expose),new_info);

	return banner_box;
}

static int misc_find_instr(const char *string, char token)
{
   int i = 0;
   for(i=0;i<strlen(string);i++)
      if(string[i] == token)
	 return i;
	   
   return -1;
}

char **misc_tokenize_string(const char *string, char delim, int *count)
{
   char **return_val = malloc(strlen(string)/2*(sizeof(char*)));
   
   int pos = 0;
   *count = 0;
   if(misc_find_instr(string,delim)==-1)
   {
      return_val[*count] = malloc(strlen(string));
      strcpy(return_val[*count],string);
      *count=*count+1;
      return return_val;
   }
   while((pos = misc_find_instr(string,delim))!=-1)
   {
      char *buffer = g_malloc0(pos);
      strncpy(buffer,string,pos);
      if(buffer!=NULL)
      {
	 return_val[*count] = g_malloc0(pos);
	 strncpy(return_val[*count],string,pos);
	 *count=*count+1;
	 if(pos != strlen(string)-1)
	 {
   	 string+=pos+1;
	 }
	 else
	 {
	    return return_val;
	 }
      }
      else
      {
	 string+=pos+1;
      }
      free(buffer);
   }
   char *buffer = g_malloc0(strlen(string));
   strncpy(buffer,string,strlen(string));
   if(buffer!=NULL)
   {
      return_val[*count] = g_malloc0(strlen(string));
      strncpy(return_val[*count],string,strlen(string));
      *count=*count+1;
   }
   free(buffer);	
   return return_val;
}

