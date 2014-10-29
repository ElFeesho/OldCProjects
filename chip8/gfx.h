#ifndef __GFX_H__
#define __GFX_H__

void gfx_init();

int gfx_draw(int x, int y, unsigned char *mem, int count);

void gfx_cls();
void gfx_flip();

#endif
