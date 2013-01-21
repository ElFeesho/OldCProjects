#ifndef __DRAW_BINDS__
#define __DRAW_BINDS__

#include <lua.h>

/* gtk_widget_* funcs */
extern int NLDT_draw_box_rel(struct lua_State *L);
extern int NLDT_draw_text_rel(struct lua_State *L);

extern int NLDT_draw_line_rel(struct lua_State *L);

extern const luaL_reg nldt_draw_methods[];

#endif
