#ifndef __LUA_VMS__
#define __LUA_VMS__

#include <gtk/gtk.h>
#include <lua.h>
#include <lauxlib.h>

typedef struct _nldt_gizmo_node
{
   struct lua_State *L; /* Lua 'VM' */
	
	GtkWidget *gizmo_cont;
	GtkWidget *title_box;
	GtkWidget *gizmo_space;
	GdkPixmap *canv_buffer;
	
	char *script_name;
	int has_focus;
	int x;
	int y;
	int w;
	int h;
	GdkColor bg_col;
	void *next;
}nldt_gizmo_node;

extern void nldt_gizmo_load(char *script_name);
extern void nldt_gizmo_reset_focus();
extern void nldt_gizmo_update(double time);
extern void nldt_gizmo_mouse_down(int x, int y);
extern void nldt_gizmo_mouse_move(int x, int y);
extern void nldt_gizmo_mouse_up(int x, int y);
extern void nldt_gizmo_key_down(int keyval);
extern void nldt_gizmo_key_up(int keyval);
extern void nldt_gizmo_shutdown();

extern nldt_gizmo_node *nldt_gizmo_lookup(struct lua_State *L);

#endif 
