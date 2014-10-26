#include <SDL/SDL.h>

static SDL_Surface *screen = NULL;

void gfx_init()
{
		SDL_Init(SDL_INIT_VIDEO);
		
		screen = SDL_SetVideoMode(64,32,24,SDL_SWSURFACE);
}

void gfx_cls()
{
	printf("CLEARING SCREEN\n");
	SDL_FillRect(screen, NULL, 0);
}

void gfx_draw(int x, int y, unsigned char *mem, int count)
{
	if(count == 0)
		count = 16;
	
	
	printf("DRAWING %dx%d %p %d\n",x, y, mem, count);
	int i;
	int j;
	
	int xpos = x*3;
	for(i=0;i<count;i++, y++)
	{
		unsigned char cb = mem[i];
		for(j = 0;j<8;j++,xpos+=3)
		{
			if(xpos>64*3)
				xpos = 0;
			if(cb&(1<<(8-i))==1)
			{
				((unsigned char*)screen->pixels)[xpos+y*(64*3)] = 0xff;
				((unsigned char*)screen->pixels)[(1+xpos)+y*(64*3)] = 0xff;
				((unsigned char*)screen->pixels)[(2+xpos)+y*(64*3)] = 0xff;
			}
			else
			{
				((unsigned char*)screen->pixels)[xpos+y*(64*3)] = 0x0;
				((unsigned char*)screen->pixels)[(1+xpos)+y*(64*3)] = 0x0;
				((unsigned char*)screen->pixels)[(2+xpos)+y*(64*3)] = 0x0;
			}
		}
	}
}