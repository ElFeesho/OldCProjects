#include <gtk/gtk.h>
#include <lua.h>
#include <lauxlib.h>

#include "gizmos.h"

/* gtk_widget_* funcs */
extern int NLDT_draw_rect(struct lua_State *L)
{
	/*
		LUA Use:
			rect([rectangle_table] | x, y, width, height, border-col, fill-col[, dest])
	*/
	int args = lua_gettop(L);
	if(args!=7 && args!=6 && lua_gettop(L)!=2 && lua_gettop(L)!=1)
	{
		lua_pushstring(L, "ERROR draw.rect usage: rectangle_table | x, y, width, height, border-col, fill-col [, dst]");
		lua_error(L);
	}
	if(lua_istable(L,1))
	{
		lua_getfield(L,1,"x");
		lua_getfield(L,1,"y");
		lua_getfield(L,1,"w");
		lua_getfield(L,1,"h");
		lua_getfield(L,1,"cols");
		lua_getfield(L,-1,"bg");
		lua_getfield(L,-2,"fg");
		int x = luaL_checkint(L,-7);
		int y = luaL_checkint(L,-6);
		int w = luaL_checkint(L,-5);
		int h = luaL_checkint(L,-4);
		GdkColor bg, fg;
		gdk_color_parse(lua_tostring(L,-2),&bg);
		gdk_color_parse(lua_tostring(L,-1),&fg);
		lua_getfield(L,1,"sw");
		int sw = -1;
		if(lua_isnil(L,-1))
		{
		
		}
		else
		{
			sw = luaL_checkint(L,-1);
		}
		if(args==2)
		{
			GdkWindow *buffer = (GdkWindow*)luaL_checkint(L,2);
			if(buffer)
			{
				GdkGC *gc = gdk_gc_new(buffer);
				gdk_gc_set_rgb_fg_color(gc,&bg);
				gdk_draw_rectangle(buffer,gc,TRUE,x,y,w,h);
				gdk_gc_set_rgb_fg_color(gc,&fg);
				if(sw > 0)
					gdk_gc_set_line_attributes(gc,sw,GDK_LINE_SOLID,GDK_CAP_ROUND,GDK_JOIN_ROUND);
				gdk_draw_rectangle(buffer,gc,FALSE,x,y,w,h);
			}
		}
		else
		{
			GdkGC *gc = NULL;
			nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
			if(owner==NULL)
			{
				fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: draw.rect)\n");
				return 0;
			}
			GdkWindow *buffer = GTK_LAYOUT(owner->gizmo_space)->bin_window;
			if(buffer!=NULL)
			{
				gc = gdk_gc_new(buffer);
				gdk_gc_set_rgb_fg_color(gc,&bg);
				gdk_draw_rectangle(buffer,gc,TRUE,x,y,w,h);
				gdk_gc_set_rgb_fg_color(gc,&fg);
				if(sw > 0)
					gdk_gc_set_line_attributes(gc,sw,GDK_LINE_SOLID,GDK_CAP_ROUND,GDK_JOIN_ROUND);
				gdk_draw_rectangle(buffer,gc,FALSE,x,y,w,h);
			}
			buffer = owner->canv_buffer;
			gc = gdk_gc_new(buffer);
			gdk_gc_set_rgb_fg_color(gc,&bg);
			gdk_draw_rectangle(buffer,gc,TRUE,x,y,w,h);
			gdk_gc_set_rgb_fg_color(gc,&fg);
			if(sw > 0)
					gdk_gc_set_line_attributes(gc,sw,GDK_LINE_SOLID,GDK_CAP_ROUND,GDK_JOIN_ROUND);
			gdk_draw_rectangle(buffer,gc,FALSE,x,y,w,h);
		}
	}
	else
	{
		int x, y, h, w;
		GdkColor border, fillcol;
		x = luaL_checkint(L,1);
		y = luaL_checkint(L,2);
		w = luaL_checkint(L,3);
		h = luaL_checkint(L,4);
		gdk_color_parse(lua_tostring(L,5),&border);
		gdk_color_parse(lua_tostring(L,6),&fillcol);
		nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
		if(owner==NULL)
		{
			fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: draw.rect)\n");
			return 0;
		}
		GdkGC *gc;
		gc = gdk_gc_new(owner->canv_buffer);

		gdk_gc_set_rgb_fg_color(gc,&fillcol);
		gdk_draw_rectangle(owner->canv_buffer,gc,1,x,y,w,h);
		gdk_gc_set_rgb_fg_color(gc,&border);
		gdk_draw_rectangle(owner->canv_buffer,gc,0,x,y,w-1,h-1);
	}	
	return 0;
}

extern int NLDT_draw_text_rel(struct lua_State *L)
{
	/*
		LUA Use:
			draw_text_rel(X,Y,Size,Colour,Text)
	*/
	if(lua_gettop(L)!=5)
	{
		lua_pushstring(L,"ERROR draw_text_rel usage X, Y, Size, Colour, Text");
		lua_error(L);
	}
	int x, y, size;
	x = luaL_checkint(L,1);
	y = luaL_checkint(L,2);
	size = luaL_checkint(L,3);
	GdkColor text_col;
	gdk_color_parse(lua_tostring(L,4),&text_col);
	unsigned int t_len = 0;
	char *text = (char*)luaL_checklstring(L,5,&t_len);
	PangoContext *pango_c = gdk_pango_context_get_for_screen (gdk_screen_get_default());
	PangoFontDescription *pdesc = pango_font_description_new();
	pango_font_description_set_family(pdesc,"arial");
	pango_font_description_set_size(pdesc,size*PANGO_SCALE);
	pango_context_set_font_description(pango_c,pdesc);
	PangoLayout *text_layout = pango_layout_new(pango_c);
	
	pango_layout_set_markup(text_layout,text,t_len);
	
	nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
	if(owner==NULL)
	{
		fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: draw.text)\n");
		return 0;
	}
	GdkGC *buff_gc = gdk_gc_new(owner->canv_buffer);
	gdk_gc_set_rgb_fg_color(buff_gc,&text_col);
	gdk_draw_layout(owner->canv_buffer,buff_gc,x,y,text_layout);
	return 0;
}

extern int NLDT_draw_line_rel(struct lua_State *L)
{
	/*
		LUA Use:
			draw_box_rel(x, y, width, height, border-col, fill-col)
	*/
	if(lua_gettop(L)!=6 && lua_gettop(L)!=7)
	{
		lua_pushstring(L, "ERROR draw_line_rel usage: x, y, x2, y2, width, line-col[, buff]");
		lua_error(L);
	}
	int x, y, x2, y2, width;
	GdkDrawable *buff;
	GdkColor line_col;
	x = luaL_checkint(L,1);
	y = luaL_checkint(L,2);
	x2 = luaL_checkint(L,3);
	y2 = luaL_checkint(L,4);
	width = luaL_checkint(L,5);
	gdk_color_parse(lua_tostring(L,6),&line_col);
	if(lua_gettop(L) == 7)
	{
		buff = (GdkDrawable*)luaL_checkint(L,7);
		GdkGC *gc;
		gc = gdk_gc_new(buff);
		gdk_gc_set_rgb_fg_color(gc,&line_col);
		gdk_gc_set_line_attributes(gc,width,GDK_LINE_SOLID,GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_draw_line(buff,gc,x,y,x2,y2);
		return 0;
	}
	nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
	if(owner==NULL)
	{
		fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: draw.line)\n");
		return 0;
	}
	GdkGC *gc;
	gc = gdk_gc_new(owner->canv_buffer);
	gdk_gc_set_rgb_fg_color(gc,&line_col);
	gdk_gc_set_line_attributes(gc,width,GDK_LINE_SOLID,GDK_CAP_ROUND,GDK_JOIN_ROUND);
	gdk_draw_line(owner->canv_buffer,gc,x,y,x2,y2);
	
	gc = gdk_gc_new(GTK_LAYOUT(owner->gizmo_space)->bin_window);
	gdk_gc_set_rgb_fg_color(gc,&line_col);
	gdk_gc_set_line_attributes(gc,width,GDK_LINE_SOLID,GDK_CAP_ROUND,GDK_JOIN_ROUND);
	gdk_draw_line(GTK_LAYOUT(owner->gizmo_space)->bin_window,gc,x,y,x2,y2);
	return 0;
}

extern int NLDT_draw_clip_line_rel(struct lua_State *L)
{
	/*
		LUA Use:
			draw_clip_line_rel(clipx, clipy, clipw, cliph, x, y, width, height, border-col, fill-col)
	*/
	if(lua_gettop(L)!=11)
	{
		lua_pushstring(L, "ERROR draw_clip_line_rel usage: fb, cx, cy, cw, ch, x, y, x2, y2, width, line-col");
		lua_error(L);
	}
	int cx, cy, cw, ch;
	
	int fb, x, y, x2, y2, width;
	GdkColor line_col;
	fb = luaL_checkint(L,1);
	cx = luaL_checkint(L,2);
	cy = luaL_checkint(L,3);
	cw = luaL_checkint(L,4);
	ch = luaL_checkint(L,5);
	
	x = luaL_checkint(L,6);
	y = luaL_checkint(L,7);
	x2 = luaL_checkint(L,8);
	y2 = luaL_checkint(L,9);
	width = luaL_checkint(L,10);
	gdk_color_parse(lua_tostring(L,11),&line_col);

	nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
	if(owner==NULL)
	{
		fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: draw.clip_line)\n");
		return 0;
	}
	GdkGC *gc;
	gc = gdk_gc_new(owner->canv_buffer);
	
	GdkRectangle clip_rect = { cx, cy, cw, ch };
	
	gdk_gc_set_clip_rectangle(gc,&clip_rect);
	gdk_gc_set_rgb_fg_color(gc,&line_col);
	gdk_gc_set_line_attributes(gc,width,GDK_LINE_SOLID,GDK_CAP_ROUND,GDK_JOIN_ROUND);
	gdk_draw_line(owner->canv_buffer,gc,x,y,x2,y2);

	if(fb)
	{
		GdkGC *gc;
		gc = gdk_gc_new(GTK_LAYOUT(owner->gizmo_space)->bin_window);
		gdk_gc_set_clip_rectangle(gc,&clip_rect);
		gdk_gc_set_rgb_fg_color(gc,&line_col);
		gdk_gc_set_line_attributes(gc,width,GDK_LINE_SOLID,GDK_CAP_ROUND,GDK_JOIN_ROUND);
		gdk_draw_line(GTK_LAYOUT(owner->gizmo_space)->bin_window,gc,x,y,x2,y2);
	}
	return 0;
}

extern int NLDT_draw_image_file(struct lua_State *L)
{
	/*
		LUA Use:
			draw_image_file(imagefile, x, y[, w, h[, srcx, srcy, srcw,srch]])
	*/
	int args = lua_gettop(L);
	if(args!=2 && args!=3 && args!=5 && args!=6 && args!=9)
	{
		lua_pushstring(L, "ERROR draw_image_file usage: \"filename\", x, y\n");
		lua_error(L);
		return 0;
	}
	GdkPixbuf *image = (GdkPixbuf*)luaL_checkint(L,1);
	int x = 0, y = 0, w = -1, h = -1, srcx = 0, srcy = 0, srcw = -1, srch = -1;
	if(args == 2)
	{
		if(!lua_istable(L,2))
		{
			lua_pushstring(L,"ERROR first arguement passed to draw.image was not a table and arguement count is 2\n");
			lua_error(L);
			return 0;
		}
		lua_getfield(L,2,"x");
		lua_getfield(L,2,"y");
		lua_getfield(L,2,"w");
		lua_getfield(L,2,"h");
		x = luaL_checkint(L,-4);
		y = luaL_checkint(L,-3);
		w = luaL_checkint(L,-2);
		h = luaL_checkint(L,-1);
	}
	else if(args == 3)
	{
		x = luaL_checkint(L,2);
		y = luaL_checkint(L,3);
	}
	else if(args == 5)
	{
		x = luaL_checkint(L,2);
		y = luaL_checkint(L,3);
		w = luaL_checkint(L,4);
		h = luaL_checkint(L,5);
	}
	else if(args == 6)
	{
		if(!lua_istable(L,2))
		{
			lua_pushstring(L,"ERROR first arguement passed to draw.image was not a table and arguement count is 6\n");
			lua_error(L);
			return 0;
		}
		lua_getfield(L,2,"x");
		lua_getfield(L,2,"y");
		lua_getfield(L,2,"w");
		lua_getfield(L,2,"h");
		x = luaL_checkint(L,-4);
		y = luaL_checkint(L,-3);
		w = luaL_checkint(L,-2);
		h = luaL_checkint(L,-1);
		srcx = luaL_checkint(L,3);
		srcy = luaL_checkint(L,4);
		srcw = luaL_checkint(L,5);
		srch = luaL_checkint(L,6);
	}
	else if(args == 9)
	{
		x = luaL_checkint(L,2);
		y = luaL_checkint(L,3);
		w = luaL_checkint(L,4);
		h = luaL_checkint(L,5);
		srcx = luaL_checkint(L,6);
		srcy = luaL_checkint(L,7);
		srcw = luaL_checkint(L,8);
		srch = luaL_checkint(L,9);
	}
	
	if(image == NULL)
	{
		lua_pushfstring(L,"ERROR Invalid Image\n");
		lua_error(L);
		return 0;
	}
	
	nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
	if(owner==NULL)
	{
		fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: draw.image)\n");
		return 0;
	}
	
	GdkGC *gc;
	gc = gdk_gc_new(owner->canv_buffer);
	if(srcw >= 0 && srch >= 0)
	{
		GdkRectangle rect;
		rect.x = 0;
		rect.y = 0;
		rect.width = srcw;
		rect.height = srch;
		gdk_gc_set_clip_rectangle(gc,&rect);	
	}
	gdk_draw_pixbuf(owner->canv_buffer,gc,image,srcx,srcx,x,y,w,h,GDK_RGB_DITHER_NONE,0,0);
	return 0;
}

extern int NLDT_draw_image_load(struct lua_State *L)
{
	/*
		LUA Use:
			draw_image_load(imagefile) returns an image pointer
	*/
	if(lua_gettop(L)!=1)
	{
		lua_pushstring(L, "ERROR draw_image_file usage: \"filename\"\n");
		lua_error(L);
	}
	char *image_name = (char*)lua_tostring(L,1);
	GdkPixbuf *image = gdk_pixbuf_new_from_file(image_name,NULL);
	if(image == NULL)
	{
		lua_pushfstring(L,"ERROR Image not found: %s\n",image_name);
		lua_error(L);
	}
	
	lua_pushinteger(L,(int)image);
	
	return 1;
}


extern int NLDT_draw_update_rect(struct lua_State *L)
{
	/*
		LUA Use:
			draw_update_rect(x, y, w, h)
	*/
	int args = lua_gettop(L);
	if(args!=4 && args!=1)
	{
		lua_pushstring(L, "ERROR update_rect usage: x, y, w, h\n");
		lua_error(L);
		return 0;
	}
	int x, y, w, h;
	if(args == 4)
	{
		x = luaL_checkint(L,1);
		y = luaL_checkint(L,2);
		w = luaL_checkint(L,3)+1;
		h = luaL_checkint(L,4)+1;
		
		nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
		if(owner==NULL)
		{
			fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: draw.update_rect)\n");
			fprintf(stderr,"Failed to look up gizmo\n");
			return 0;
		}
		
		if(GTK_LAYOUT(owner->gizmo_space)->bin_window!=NULL)
		{
			gdk_draw_rectangle(owner->canv_buffer, gdk_gc_new(owner->canv_buffer), 0,0,0,owner->w-1,owner->h-1);
			gdk_draw_drawable(GTK_LAYOUT(owner->gizmo_space)->bin_window,gdk_gc_new(GTK_LAYOUT(owner->gizmo_space)->bin_window),owner->canv_buffer,x,y,x,y,w,h);			
		}
	}
	else
	{
		lua_getfield(L,1,"x");
		lua_getfield(L,1,"y");
		lua_getfield(L,1,"w");
		lua_getfield(L,1,"h");
		x = luaL_checkint(L,-4);
		y = luaL_checkint(L,-3);
		w = luaL_checkint(L,-2);
		h = luaL_checkint(L,-1);
		
		nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
		if(owner==NULL)
		{
			fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: draw.update_rect)\n");
			return 0;
		}
		
		if(GTK_LAYOUT(owner->gizmo_space)->bin_window!=NULL)
		{
			gdk_draw_rectangle(owner->canv_buffer, gdk_gc_new(owner->canv_buffer), 0,0,0,owner->w-1,owner->h-1);
			gdk_draw_drawable(GTK_LAYOUT(owner->gizmo_space)->bin_window,gdk_gc_new(GTK_LAYOUT(owner->gizmo_space)->bin_window),owner->canv_buffer,x,y,x,y,w,h);
		}
	}
	return 0;
}

extern int NLDT_draw_clear(struct lua_State *L)
{
	/*
		LUA Use:
			draw_update_rect(x, y, w, h)
	*/
	if(lua_gettop(L)!=0)
	{
		lua_pushstring(L, "ERROR update_rect usage: x, y, w, h\n");
		lua_error(L);
	}
	
	lua_getglobal(L,"gizmo");
	lua_getfield(L,-1,"bg_col");
	GdkColor clearcol;
	gdk_color_parse(lua_tostring(L,-1),&clearcol);
	
	int w, h;
	lua_getglobal(L,"gizmo");
	lua_getfield(L,-1,"w");
	lua_getfield(L,-2,"h");
	
	w = luaL_checkint(L,-2);
	h = luaL_checkint(L,-1);
	
	nldt_gizmo_node *owner = nldt_gizmo_lookup(L);
	if(owner==NULL)
	{
		fprintf(stderr,"ERROR: Attempted to draw before initialisation (Offender: draw.clear)\n");
		return 0;
	}
	GdkGC *gc = gdk_gc_new(owner->canv_buffer);
	gdk_gc_set_rgb_fg_color(gc,&clearcol);
	
	gdk_draw_rectangle(owner->canv_buffer,gc,1,0,0,w,h);	
	return 0;
}

const luaL_reg nldt_draw_methods[] =
{
	{ "rect", NLDT_draw_rect },
	{ "text", NLDT_draw_text_rel },
	{ "line", NLDT_draw_line_rel },
	{ "clip_line_rel", NLDT_draw_clip_line_rel },
	{ "image_load", NLDT_draw_image_load },
	{ "image", NLDT_draw_image_file },
	{ "update_rect", NLDT_draw_update_rect },
	{ "clear", NLDT_draw_clear },
	{ 0, 0 }
};

