#include <gtk/gtk.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "gizmos.h"
#include "gtk_binds.h"
#include "draw_binds.h"
#include "sound.h"
#include "codeedit.h"

extern void main_window_add_child(GtkWidget *child, int x, int y);

static nldt_gizmo_node *nldt_gizmo_list = NULL;

static void nldt_gizmo_reload(GtkWidget *button, nldt_gizmo_node *gizmo);

static gboolean nldt_gizmo_canvas_expose(GtkWidget *canvas, GdkEventExpose *ev, gpointer user_data)
{
	nldt_gizmo_node *temp = (nldt_gizmo_node*)user_data;
	lua_getglobal(temp->L,"on_expose");
	int result = lua_pcall(temp->L,0,0,0);
	if(result!=0 && result!=2)
	{
		fprintf(stderr,"Failed to run expose function on %s script\nError: %s\n",temp->script_name,lua_tostring(temp->L,-1));
	}
	gdk_draw_rectangle(temp->canv_buffer, gdk_gc_new(temp->canv_buffer), 0,0,0,temp->w-1,temp->h-1);
	gdk_draw_drawable(GTK_LAYOUT(canvas)->bin_window, gdk_gc_new(GTK_LAYOUT(canvas)->bin_window), temp->canv_buffer,0,0,0,0,temp->w,temp->h);
	return FALSE;
}

static gboolean nldt_gizmo_canvas_mouse_down(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	nldt_gizmo_reset_focus();
	nldt_gizmo_node *temp = (nldt_gizmo_node*)user_data;
	temp->has_focus = TRUE;
	lua_getglobal(temp->L,"on_mouse_down");
	lua_pushinteger(temp->L,ev->x);
	lua_pushinteger(temp->L,ev->y);
	lua_pushinteger(temp->L,ev->button);
	int result = lua_pcall(temp->L,3,0,0);
	if(result && result!=2)
	{
		fprintf(stderr,"Failed to run on_mouse_down function on %s script\nError: %s\n",temp->script_name,lua_tostring(temp->L,-1));
	}
	return TRUE; //Don't allow any other mouse click's to be capture by other signals
}

static gboolean nldt_gizmo_canvas_mouse_move(GtkWidget *canvas, GdkEventMotion *ev, gpointer user_data)
{
	nldt_gizmo_reset_focus();
	nldt_gizmo_node *temp = (nldt_gizmo_node*)user_data;
	temp->has_focus = TRUE;
	lua_getglobal(temp->L,"on_mouse_move");
	lua_pushinteger(temp->L,ev->x);
	lua_pushinteger(temp->L,ev->y);
	int button = 0;
	if((ev->state & GDK_BUTTON1_MASK) == GDK_BUTTON1_MASK)
	{
		button = 1;
	}
	else if((ev->state & GDK_BUTTON2_MASK) == GDK_BUTTON2_MASK)
	{
		button = 2;
	}
	else if((ev->state & GDK_BUTTON3_MASK) == GDK_BUTTON3_MASK)
	{
		button = 3;
	}
	lua_pushinteger(temp->L,button);
	int result = lua_pcall(temp->L,3,0,0);
	if(result && result!=2)
	{
		fprintf(stderr,"Failed to run on_mouse_move function on %s script\nError: %s\n",temp->script_name,lua_tostring(temp->L,-1));
	}
	return TRUE; //Don't allow any other mouse click's to be capture by other signals
}

static gboolean nldt_gizmo_canvas_mouse_up(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	nldt_gizmo_reset_focus();
	nldt_gizmo_node *temp = (nldt_gizmo_node*)user_data;
	temp->has_focus = TRUE;
	lua_getglobal(temp->L,"on_mouse_up");
	lua_pushinteger(temp->L,ev->x);
	lua_pushinteger(temp->L,ev->y);
	lua_pushinteger(temp->L,ev->button);
	int result = lua_pcall(temp->L,3,0,0);
	if(result && result!=2)
	{
		fprintf(stderr,"Failed to run on_mouse_up function on %s script\n\nError: %s\n",temp->script_name,lua_tostring(temp->L,-1));
	}
	return TRUE; //Don't allow any other mouse click's to be capture by other signals
}

static void nldt_gizmo_edit(GtkWidget *button, nldt_gizmo_node *gizmo)
{
	//ce_show(gizmo->script_name);
}

static void nldt_gizmo_add_user_table(lua_State *L)
{
	//int users = 0;
	//for(users=0;users<users_count;users++)
	//{
	lua_newtable(L);
	lua_newtable(L); 
	lua_newtable(L);
	lua_rawseti(L, -2, 0); // users); 
	lua_setglobal(L, "users");
	
	lua_getglobal(L,"users");
	lua_pushnumber(L,0); //users); 
	lua_gettable(L,-2);			
	lua_pushstring(L,"cols");	
	lua_newtable(L);				
	lua_settable(L,-3);			
	
	lua_getglobal(L,"users");
	lua_pushnumber(L,0);
	lua_gettable(L,-2);
	lua_pushstring(L,"Feesh!");
	lua_setfield(L,-2,"name");
	lua_pushstring(L,"cols");
	lua_gettable(L,-2);
	lua_pushstring(L,"#09f");
	lua_setfield(L,-2,"fg");	
	lua_pushstring(L,"#f90");
	lua_setfield(L,-2,"bg");
	//}
}

static nldt_gizmo_node *nldt_gizmo_create(char *script_name)
{
	nldt_gizmo_node *gizmo = g_new(nldt_gizmo_node,1); // Return value
	
	gizmo->gizmo_cont = gtk_vbox_new(0,0);
	gizmo->title_box = gtk_event_box_new();
	GtkWidget *hbox = gtk_hbox_new(0,0);
	GtkWidget *edit, *reload;
	edit = gtk_button_new_with_label("Edit");
	reload = gtk_button_new_with_label("Reload");
	
	g_signal_connect(G_OBJECT(edit),"clicked",G_CALLBACK(nldt_gizmo_edit),gizmo);
	g_signal_connect(G_OBJECT(reload),"clicked",G_CALLBACK(nldt_gizmo_reload),gizmo);
	
	gtk_container_add(GTK_CONTAINER(gizmo->title_box),hbox);
	gtk_box_pack_end(GTK_BOX(hbox),edit,0,0,0);
	gtk_box_pack_end(GTK_BOX(hbox),reload,0,0,0);
	gtk_box_pack_start(GTK_BOX(gizmo->gizmo_cont),gizmo->title_box,0,0,0);
	
	gizmo->gizmo_space = gtk_layout_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(gizmo->gizmo_cont), gizmo->gizmo_space,0,0,0);
	
	gizmo->L = lua_open();
	gizmo->next = NULL;
	gizmo->has_focus = TRUE;
	gizmo->script_name = g_strdup(script_name);
	
	luaL_openlibs(gizmo->L);
	
	luaL_register(gizmo->L,"gtk",nldt_gtk_methods);
	luaL_register(gizmo->L,"draw",nldt_draw_methods);
	luaL_register(gizmo->L,"sound",nldt_sound_methods);
	
	lua_newtable(gizmo->L);
	lua_setglobal(gizmo->L,"gizmo");
	lua_getglobal(gizmo->L,"gizmo");
	lua_pushnumber(gizmo->L,0);
	lua_setfield(gizmo->L,-2,"current_user");
	
	nldt_gizmo_add_user_table(gizmo->L);

	int file_stat = luaL_loadfile(gizmo->L, script_name);
	if(file_stat)
	{
		fprintf(stderr,"Failed loading %s gizmo\n", script_name);
		fprintf(stderr,"Lua Error: %s\n",lua_tostring(gizmo->L,-1));
		lua_close(gizmo->L);
		gizmo = NULL;
		return gizmo;
	}
	
	int result = lua_pcall(gizmo->L, 0, 0, 0);
	if(result)
	{
		fprintf(stderr,"Failed to run %s\n",script_name);
		fprintf(stderr,"Lua Error: %s\n",lua_tostring(gizmo->L,-1));
		lua_close(gizmo->L);
		gizmo = NULL;
		return gizmo;
	}

	lua_getglobal(gizmo->L,"gizmo");
	lua_getfield(gizmo->L,-1,"x");
	lua_getfield(gizmo->L,-2,"y");
	lua_getfield(gizmo->L,-3,"w");
	lua_getfield(gizmo->L,-4,"h");
	lua_getfield(gizmo->L,-5,"bg_col");
	lua_remove(gizmo->L,-6);
	
	//After init function, x, y, width and height should be set so
	//store em in the gizmo struct
	gizmo->x = lua_tointeger(gizmo->L,-5);
	gizmo->y = lua_tointeger(gizmo->L,-4);
	gizmo->w = lua_tointeger(gizmo->L,-3);
	gizmo->h = lua_tointeger(gizmo->L,-2);
	gdk_color_parse(lua_tostring(gizmo->L,-1),&gizmo->bg_col);
	
	gizmo->canv_buffer = gdk_pixmap_new(NULL,gizmo->w, gizmo->h, 24);
	
	GdkGC *buff_gc = gdk_gc_new(gizmo->canv_buffer);
	
	gdk_gc_set_rgb_fg_color(buff_gc,&gizmo->bg_col);
	
	gdk_draw_rectangle(gizmo->canv_buffer,buff_gc,1,0,0,gizmo->w,gizmo->h);
	
	gtk_widget_set_size_request(gizmo->gizmo_space,gizmo->w,gizmo->h);
			
	gtk_widget_add_events(gizmo->gizmo_space,GDK_KEY_PRESS_MASK|GDK_POINTER_MOTION_MASK|GDK_BUTTON1_MOTION_MASK|GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK);
	g_signal_connect(G_OBJECT(gizmo->gizmo_space),"expose-event",G_CALLBACK(nldt_gizmo_canvas_expose),gizmo);
	g_signal_connect(G_OBJECT(gizmo->gizmo_space),"button-press-event",G_CALLBACK(nldt_gizmo_canvas_mouse_down),gizmo);
	g_signal_connect(G_OBJECT(gizmo->gizmo_space),"button-release-event",G_CALLBACK(nldt_gizmo_canvas_mouse_up),gizmo);
	g_signal_connect(G_OBJECT(gizmo->gizmo_space),"motion-notify-event",G_CALLBACK(nldt_gizmo_canvas_mouse_move),gizmo);
	
	lua_getglobal(gizmo->L,"gizmo");
	lua_getfield(gizmo->L,-1,"name");
	lua_remove(gizmo->L,-2);
	char *app_name = (char*)lua_tostring(gizmo->L,-1);
	if(app_name)
	{
		gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(app_name),0,0,0);
	}
	else
	{
		gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Untitled Gizmo"),0,0,0);
	}
	main_window_add_child(gizmo->gizmo_cont,gizmo->x, gizmo->y);
	lua_getglobal(gizmo->L,"gizmo");
	lua_pushnumber(gizmo->L,(int)gizmo->canv_buffer);
	lua_setfield(gizmo->L,-2,"bg_buf");
	lua_pushnumber(gizmo->L,(int)GTK_LAYOUT(gizmo->gizmo_space)->bin_window);
	lua_setfield(gizmo->L,-2,"fg_buf");
		
	return gizmo;
}

extern void nldt_gizmo_load(char *script_name)
{
	if(!nldt_gizmo_list)
	{
		nldt_gizmo_list = nldt_gizmo_create(script_name);
		lua_getglobal(nldt_gizmo_list->L,"on_init");
		int result = lua_pcall(nldt_gizmo_list->L,0,0,0);
		if(result && result!=2)
		{
			fprintf(stderr,"Failed initialising %s\n",script_name);
			fprintf(stderr,"Clearing up\n");
			fprintf(stderr,"Lua Error: %s\n",lua_tostring(nldt_gizmo_list->L,-1));
			lua_close(nldt_gizmo_list->L);
			return;
		}
	}
}

extern void nldt_gizmo_reset_focus()
{
	nldt_gizmo_node *temp = nldt_gizmo_list;
	while(temp!=NULL)
	{
		temp->has_focus = FALSE;
		temp = temp->next;
	}
}

extern void nldt_gizmo_update(double time)
{
	nldt_gizmo_node *temp = nldt_gizmo_list;
	int result = 0;
	while(temp!=NULL)
	{
		lua_getglobal(temp->L,"on_update");
		lua_pushnumber(temp->L,time);
		result = lua_pcall(temp->L,1,0,0);
		if(result)
		{
			fprintf(stderr,"Failed to run update function on %s script\n",temp->script_name);
			fprintf(stderr,"Lua Error: %s\n",lua_tostring(temp->L,-1));
		}
		temp = temp->next;		
	}
}

extern void nldt_gizmo_mouse_down(int x, int y)
{
	nldt_gizmo_node *temp = nldt_gizmo_list;
	int result = 0;
	while(temp!=NULL)
	{
		lua_getglobal(temp->L,"on_mouse_down");
		lua_pushinteger(temp->L,x);
		lua_pushinteger(temp->L,y);
		result = lua_pcall(temp->L,2,0,0);
		if(result)
		{
			fprintf(stderr,"Failed to run mouse down function on %s script\n",temp->script_name);
		}
		temp = temp->next;		
	}	
}

extern void nldt_gizmo_mouse_move(int x, int y)
{
	nldt_gizmo_node *temp = nldt_gizmo_list;
	int result = 0;
	while(temp!=NULL)
	{
		lua_getglobal(temp->L,"on_mouse_move");
		lua_pushinteger(temp->L,x);
		lua_pushinteger(temp->L,y);
		result = lua_pcall(temp->L,2,0,0);
		if(result)
		{
			fprintf(stderr,"Failed to run mouse move function on %s script\n",temp->script_name);
		}
		temp = temp->next;		
	}
}

extern void nldt_gizmo_mouse_up(int x, int y)
{
	nldt_gizmo_node *temp = nldt_gizmo_list;
	int result = 0;
	while(temp!=NULL)
	{
		lua_getglobal(temp->L,"on_mouse_up");
		lua_pushinteger(temp->L,x);
		lua_pushinteger(temp->L,y);
		result = lua_pcall(temp->L,2,0,0);
		if(result)
		{
			fprintf(stderr,"Failed to run update function on %s script\n",temp->script_name);
		}
		temp = temp->next;
	}
}

extern void nldt_gizmo_key_down(int keyval)
{
	nldt_gizmo_node *temp = nldt_gizmo_list;
	int result = 0;
	while(temp!=NULL)
	{
		if(temp->has_focus)
		{
			lua_getglobal(temp->L,"on_key_down");
			lua_pushinteger(temp->L,keyval);
			result = lua_pcall(temp->L,1,0,0);
			if(result)
			{
				fprintf(stderr,"Failed to run key down function on %s script\n",temp->script_name);
			}		
		}
		temp = temp->next;
	}
}

extern void nldt_gizmo_key_up(int keyval)
{
	nldt_gizmo_node *temp = nldt_gizmo_list;
	int result = 0;
	while(temp!=NULL)
	{
		if(temp->has_focus)
		{
			lua_getglobal(temp->L,"on_key_up");
			lua_pushinteger(temp->L,keyval);
			result = lua_pcall(temp->L,1,0,0);
			if(result)
			{
				fprintf(stderr,"Failed to run key up function on %s script\n",temp->script_name);
			}
		}
		temp = temp->next;
	}
}

extern void nldt_gizmo_shutdown()
{
	nldt_gizmo_node *temp = nldt_gizmo_list;
	while(temp!=NULL)
	{
		lua_getglobal(temp->L,"on_destroy");
		lua_pcall(temp->L,0,0,0);
		lua_close(temp->L);
		temp = temp->next;
	}
}

extern nldt_gizmo_node *nldt_gizmo_lookup(struct lua_State *L)
{
	nldt_gizmo_node *temp = nldt_gizmo_list;
	while(temp!=NULL)
	{
		if(temp->L == L)
			return temp;
		temp = temp->next;
	}
	return NULL;
}


extern void nldt_gizmo_remove(nldt_gizmo_node *gizmo)
{
	nldt_gizmo_node *temp = nldt_gizmo_list;
	nldt_gizmo_node *prev = NULL;
	while(temp)
	{
		if(temp == gizmo)
		{
			if(prev != NULL)
			{
				if(temp->next!=NULL)
					prev->next = temp->next;
				else
					prev->next = NULL;
			}
			if(prev==NULL)
				nldt_gizmo_list = temp->next;
			g_free(temp);
			temp = NULL;
			
			return;
		}
		prev = temp;
		temp = temp->next;
	}
}

static void nldt_gizmo_reload(GtkWidget *button, nldt_gizmo_node *gizmo)
{
	char *script_name = g_strdup(gizmo->script_name);
	lua_close(gizmo->L);
	gtk_widget_destroy(gizmo->gizmo_space);
	nldt_gizmo_remove(gizmo);
	nldt_gizmo_load(script_name);
}
