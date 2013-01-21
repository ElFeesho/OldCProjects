#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>


GdkPixbuf *c_image = NULL;
int img_x, img_y, img_w, img_h;


lua_State *L; // <- Eventually have a linked list for each script?

static int load_image(lua_State *l)
{
	int file_len;
	char *image_file = (char*)luaL_checklstring(l, 1, &file_len);
	img_x = luaL_checkint(l,2);
	img_y = luaL_checkint(l,3);
	img_w = luaL_checkint(l,4);
	img_h = luaL_checkint(l,5);
	c_image = gdk_pixbuf_new_from_file_at_scale(image_file,img_w,img_h,0,NULL);
	return 1;
}

gboolean window_on_key(GtkWidget *window, GdkEventKey *ev, gpointer user_data)
{
	lua_getglobal(L,"on_key_press");
	if(ev->keyval == GDK_Left)
		lua_pushinteger(L,31);
	if(ev->keyval == GDK_Right)
		lua_pushinteger(L,32);
	if(ev->keyval == GDK_Up)
		lua_pushinteger(L,33);
	if(ev->keyval == GDK_Down)
		lua_pushinteger(L,34);

	lua_pcall(L,1,0,0);

	return TRUE;
}

gboolean canvas_on_click(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{	
	lua_getglobal(L,"on_click");
	lua_pushinteger(L,ev->x);
	lua_pushinteger(L,ev->y);
	int res = lua_pcall(L,2,0,0);
	if(res)
	{
		printf("Failed to mouse click\n");
	}
	return TRUE;
}

gboolean canvas_on_release(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{	
	lua_getglobal(L,"on_mouse_up");
	lua_pushinteger(L,ev->x);
	lua_pushinteger(L,ev->y);
	int res = lua_pcall(L,2,0,0);
	if(res)
	{
		printf("Failed to mouse click\n");
	}
	return TRUE;
}

gboolean canvas_on_move(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data)
{	
	lua_getglobal(L,"on_mouse_move");
	lua_pushinteger(L,ev->x);
	lua_pushinteger(L,ev->y);
	int res = lua_pcall(L,2,0,0);
	if(res)
	{
		printf("Failed to mouse Move\n");
	}
	return TRUE;
}

gboolean window_delete(GtkWidget *window, gpointer user_data)
{
	lua_close(L);
	gtk_exit(0);
	return FALSE;
}

gboolean interp(GtkWidget *canvas)
{
	static GdkPixmap *bg_buf = NULL;
	if(bg_buf == NULL)
	{
		bg_buf = gdk_pixmap_new(NULL,500,400,24);
	}
	GdkColor black;
	GdkColor red;
	gdk_color_parse("#000000",&black);
	gdk_color_parse("#FF0000",&red);
	GdkGC *gc = gdk_gc_new(bg_buf);
	lua_getglobal(L,"on_update");
	int res = lua_pcall(L,0,0,0);
	if(res)
	{
		printf("Error calling on_update\n");
	}
	
	int x, y, w, h;
	lua_getglobal(L,"x");
	x = lua_tointeger(L,-1);
	lua_getglobal(L,"y");
	y = lua_tointeger(L,-1);
	lua_getglobal(L,"w");
	w = lua_tointeger(L,-1);
	lua_getglobal(L,"h");
	h = lua_tointeger(L,-1);
	gdk_gc_set_rgb_fg_color(gc,&black);
	gdk_draw_rectangle(bg_buf,gc,1,0,0,500,400);
	gdk_gc_set_rgb_fg_color(gc,&red);
	gdk_draw_rectangle(bg_buf,gc,1,x,y,w,h);
	if(c_image!=NULL)
		gdk_draw_pixbuf(bg_buf,gc,c_image,0,0,img_x,img_y,img_w,img_h,GDK_RGB_DITHER_NONE,0,0);
	gdk_draw_drawable(canvas->window,gdk_gc_new(canvas->window),bg_buf,0,0,0,0,500,400);
	
	return TRUE;
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);

	GtkWidget *window, *canvas;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	canvas = gtk_drawing_area_new();
	gtk_widget_add_events(canvas,GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK|GDK_BUTTON_MOTION_MASK);
	gtk_widget_set_size_request(canvas,500,400);

	gtk_container_add(GTK_CONTAINER(window),canvas);

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_exit),NULL);
	g_signal_connect(G_OBJECT(window),"key-press-event",G_CALLBACK(window_on_key),NULL);
	g_signal_connect(G_OBJECT(canvas),"button-press-event",G_CALLBACK(canvas_on_click),NULL);
	g_signal_connect(G_OBJECT(canvas),"button-release-event",G_CALLBACK(canvas_on_release),NULL);
	g_signal_connect(G_OBJECT(canvas),"motion-notify-event",G_CALLBACK(canvas_on_move),NULL);
	gtk_widget_show_all(window);

	L = lua_open();
	luaL_openlibs(L);
	lua_register(L,"load_image",load_image);
	int file_stat = luaL_loadfile(L,"script.lua");
	
	if(file_stat)
	{
		fprintf(stderr,"Failed to load the script\n");
		return -1;
	}
	int result = lua_pcall(L, 0, 0, 0);
	if(result)
	{
		fprintf(stderr,"Failed to run the script\n");
		exit(1);
	}
	lua_getglobal(L,"initialise");
	lua_pushinteger(L,200);
	lua_pushinteger(L,200);
	lua_pushinteger(L,30);
	lua_pushinteger(L,30);
	result = lua_pcall(L,4,0,0);
	if(result)
	{
		printf("Failed calling initialise\n");
	}
	g_timeout_add(20,(GSourceFunc)interp,canvas);

	gtk_main();
	return 0;
}
