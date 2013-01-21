#ifndef __GTK_BINDS__
#define __GTK_BINDS__

#include <lua.h>

/* gtk_widget_* funcs */
extern int NLDT_gtk_show(struct lua_State *L);
extern int NLDT_gtk_hide(struct lua_State *L);
extern int NLDT_gtk_set_position(struct lua_State *L);
extern int NLDT_gtk_destroy(struct lua_State *L);


/* Widget creators */
extern int NLDT_gtk_button_new(struct lua_State *L);
extern int NLDT_gtk_label_new(struct lua_State *L);

/* Misc Funcs */

extern int NLDT_gtk_signal_connect(struct lua_State *L);

extern const luaL_reg nldt_gtk_methods[];

#endif
