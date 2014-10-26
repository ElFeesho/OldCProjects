#include <SDL/SDL.h>

static SDL_Surface *screen = NULL;

void gfx_init()
{
	SDL_Init(SDL_INIT_VIDEO);
		
	screen = SDL_SetVideoMode(640, 320, 24, SDL_SWSURFACE);
}

void gfx_cls()
{
	printf("CLEARING SCREEN\n");
	SDL_FillRect(screen, NULL, 0);
}

int gfx_draw_pixel_on(int x, int y)
{
	SDL_Rect pos = { x, y, 10, 10 };
	SDL_FillRect(screen, &pos, 0xffffff);
	// TODO: Return 1 if pixels are altered
	return 0;
}

int gfx_draw_pixel_off(int x, int y)
{
	SDL_Rect pos = { x, y, 10, 10 };
	SDL_FillRect(screen, &pos, 0xff000000);
	return 0;
}

int gfx_draw(int x, int y, unsigned char *mem, int count)
{
	if(count == 0)
		count = 16;
	
	printf("DRAWING %dx%d %p %d\n",x, y, mem, count);
	int i;
	int j;

	for(j = 0; j < count; j++)
	{
		unsigned char byte = mem[j];
		printf("'");
		for(i = 0; i < 8; i++)
		{
			printf("%c", byte & (1 << (8-i))?'X':' ');
		}
		printf("' %X\n", byte);
	}
	
	int shouldFlip = 0;

	for(i=0; i < count; i++, y++)
	{
		unsigned char cb = mem[i];
		for(j = 0;j<8;j++)
		{
			if(cb & (1 <<(8 - i)) == 1)
			{
				shouldFlip |= gfx_draw_pixel_on(x+j, y);
			}
			else
			{
				shouldFlip |= gfx_draw_pixel_off(x+j, y);
			}
		}
	}

	return shouldFlip;
}