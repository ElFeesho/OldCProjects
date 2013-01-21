#ifndef __SOUND_H__
#define __SOUND_H__

#include <lua.h>

extern void sound_init();
extern void sound_deinit();
extern const luaL_reg nldt_sound_methods[];

#endif
