#ifndef __GFX_H__
#define __GFX_H__

extern void gfx_init();

extern int gfx_draw(int x, int y, unsigned char *mem, int count);

extern void gfx_cls();
extern void gfx_flip();

#endif
