#include <gtk/gtk.h>
#include <lua.h>
#include <lauxlib.h>

#include "gizmos.h"

typedef struct _callback_data
{
	nldt_gizmo_node *gizmo;
	char *func_name;
}callback_data;

static void NLDT_button_click_callback(GtkWidget *button, gpointer user_data)
{
	/*
		Callbacks will have to be all thought through...
		"It's a living"
	*/
	callback_data *cback = (callback_data*)user_data;
	lua_getglobal(cback->gizmo->L,cback->func_name);
	lua_pushinteger(cback->gizmo->L,(int)button);
	int result = lua_pcall(cback->gizmo->L,1,0,0);
	if(result)
	{
		fprintf(stderr,"ERROR callback: %s failed to launch\n", cback->func_name);
		fprintf(stderr,"Lua Error: %s\n",lua_tostring(cback->gizmo->L,-1));
	}
}

extern int NLDT_gtk_show(struct lua_State *L)
{
	/*
		LUA Use:
			gtk_show(widget)
	*/
	if(lua_gettop(L)!=1)
	{
		lua_pushstring(L,"ERROR gtk_show usage widget\n");
		lua_error(L);
	}
	GtkWidget *widget = (GtkWidget*)luaL_checkint(L,1);
	gtk_widget_show(widget);
	return 0;
}

extern int NLDT_gtk_hide(struct lua_State *L)
{
	/*
		LUA Use:
			gtk_hide(widget)
	*/
	if(lua_gettop(L)!=1)
	{
		lua_pushstring(L,"ERROR gtk_hide usage widget\n");
		lua_error(L);
	}
	GtkWidget *widget = (GtkWidget*)luaL_checkint(L,1);
	gtk_widget_hide(widget);
	return 0;
}

extern int NLDT_gtk_set_position(struct lua_State *L)
{
	/*
		LUA Use:
			gtk_set_position(widget, x, y)
	*/
	if(lua_gettop(L)!=3)
	{
		lua_pushstring(L,"ERROR gtk_set_position usage widget, x, y\n");
		lua_error(L);
	}
	
	GtkWidget *widget;	
	widget = (GtkWidget*)luaL_checkint(L,1);
	
	if(widget==NULL)
	{
		lua_pushstring(L,"ERROR widget is Nil");
		lua_error(L);
	}
	
	int x, y;
	x = luaL_checkint(L,2);
	y = luaL_checkint(L,3);
	
	nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
	if(owner==NULL)
	{
		fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: gtk.set_position)\n");
		return 0;
	}
	gtk_layout_move(GTK_LAYOUT(owner->gizmo_space),widget,x,y);
	
	return 0;
}

extern int NLDT_gtk_destroy(struct lua_State *L)
{
	/*
		LUA Use:
			gtk_destroy(widget)
	*/
	
	if(lua_gettop(L)!=1)
	{
		lua_pushstring(L,"ERROR gtk_destroy usage widget\n");
		lua_error(L);
	}
	GtkWidget *widget = (GtkWidget*)luaL_checkint(L,1);
	gtk_widget_destroy(widget);
	return 0;
}


/* Widget creators */
extern int NLDT_gtk_button_new(struct lua_State *L)
{
	/*
		LUA Use:
			gtk_button_new("Some Text",Width, Height)
	*/
	if(lua_gettop(L)!=5)
	{
		lua_pushstring(L,"ERROR gtk_button_new usage \"Label\", Xpos, Ypos, Width, Height\n");
		lua_error(L); 
	}
	
	char *label = (char*)lua_tostring(L,1);
	
	int x,y,w, h;
	x = luaL_checkint(L,2);
	y = luaL_checkint(L,3);
	w = luaL_checkint(L,4);
	h = luaL_checkint(L,5);
	
	GtkWidget *button = gtk_button_new_with_label(label);
	gtk_widget_set_size_request(button,w,h);
	
	nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
	if(owner==NULL)
	{
		fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: gtk.button_new)\n");
		return 0;
	}
	gtk_layout_put(GTK_LAYOUT(owner->gizmo_space),button,x,y);
	gtk_widget_show(button);
	lua_pushinteger(L,(int)button);
	
	return 1;
}

extern int NLDT_gtk_button_set_text(struct lua_State *L)
{
	/*
		LUA Use:
			gtk_button_set_text(label,"Some Text")
	*/
	if(lua_gettop(L)!=2)
	{
		lua_pushstring(L,"ERROR gtk_label_set_text usage label, \"Text\"\n");
		lua_error(L);
	}
	GtkWidget *widget = (GtkWidget*)luaL_checkint(L,1);
	char *label_text = (char*)lua_tostring(L,2);
	gtk_button_set_label(GTK_BUTTON(widget),label_text);
	return 0;
}


extern int NLDT_gtk_button_new_from_image(struct lua_State *L)
{
	/*
		LUA Use:
			gtk_button_new("Some Text",Width, Height)
	*/
	if(lua_gettop(L)!=3)
	{
		lua_pushstring(L,"ERROR gtk_button_new_from_image usage \"filename\", Width, Height\n");
		lua_error(L); 
	}
	
	char *file_name = (char*)lua_tostring(L,1);
	
	int w, h;
	w = luaL_checkint(L,2);
	h = luaL_checkint(L,3);
	
	GtkWidget *button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button),gtk_image_new_from_file(file_name));
	gtk_widget_set_size_request(button,w,h);
	
	nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
	if(owner==NULL)
	{
		fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: gtk.button_new_from_image)\n");
		return 0;
	}
	gtk_layout_put(GTK_LAYOUT(owner->gizmo_space),button,10,10);
	gtk_widget_show(button);
	lua_pushinteger(L,(int)button);
	
	return 1;
}

extern int NLDT_gtk_label_new(struct lua_State *L)
{
	/*
		LUA Use:
			gtk_label_new("Some Text")
	*/
	if(lua_gettop(L)!=1)
	{
		lua_pushstring(L,"ERROR gtk_label_new usage \"Label\"\n");
		lua_error(L);
	}
	
	char *label_text = (char*)lua_tostring(L,1);
	
	GtkWidget *label = gtk_label_new(label_text);
	
	nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
	if(owner==NULL)
	{
		fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: gtk.label_new)\n");
		return 0;
	}
	gtk_layout_put(GTK_LAYOUT(owner->gizmo_space),label,10,10);
	gtk_widget_show(label);
	lua_pushinteger(L,(int)label);
	return 1;
}

extern int NLDT_gtk_label_set_text(struct lua_State *L)
{
	/*
		LUA Use:
			gtk_label_set_text(label,"Some Text")
	*/
	if(lua_gettop(L)!=2)
	{
		lua_pushstring(L,"ERROR gtk_label_set_text usage label, \"Text\"\n");
		lua_error(L);
	}
	GtkWidget *widget = (GtkWidget*)luaL_checkint(L,1);
	char *label_text = (char*)lua_tostring(L,2);
	gtk_label_set_text(GTK_LABEL(widget),label_text);
	return 0;
}

/* Misc Funcs */

extern int NLDT_gtk_signal_connect(struct lua_State *L)
{
	/*
		LUA Use:
			gtk_signal_connect(widget, "signal", "lua_function")
	*/
	if(lua_gettop(L)!=3)
	{
		lua_pushstring(L,"ERROR gtk_signal_connect usage widget, \"signal\", \"lua_func\"\n");
		lua_error(L);
	}
	GtkWidget *widget = (GtkWidget*)luaL_checkint(L,1);
	char *callback = (char*)lua_tostring(L,2);
	char *lua_func = (char*)lua_tostring(L,3);
	callback_data *cback = g_new(callback_data,1);
	cback->gizmo = nldt_gizmo_lookup(L);
	cback->func_name = g_strdup(lua_func);
	if(GTK_IS_BUTTON(widget))
	{
		g_signal_connect(G_OBJECT(widget),callback,G_CALLBACK(NLDT_button_click_callback),cback);
	}	
	if(GTK_IS_LABEL(widget))
	{
		printf("hehe, silly rabbit, callbacks are for buttons!\n");
	}
	return 0;
}

const luaL_reg nldt_gtk_methods[] = {
	{"show",   NLDT_gtk_show},
	{"hide", NLDT_gtk_hide},
	{"set_position", NLDT_gtk_set_position},
	{"destroy", NLDT_gtk_destroy},
	{"label_new", NLDT_gtk_label_new},
	{"label_set_text",NLDT_gtk_label_set_text},
	{"button_new", NLDT_gtk_button_new},
	{"button_set_text", NLDT_gtk_button_set_text },
	{"button_new_from_image", NLDT_gtk_button_new_from_image},
	{"signal_connect", NLDT_gtk_signal_connect},
	{ 0, 0 }
};
