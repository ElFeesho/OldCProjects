#include <SDL/SDL.h>
#include "gfx.h"

static SDL_Surface *screen = NULL;

unsigned char *screenBuffer;

int gfx_draw_pixel_on();

void gfx_init()
{
	SDL_Init(SDL_INIT_VIDEO);

	screen = SDL_SetVideoMode(640, 320, 24, SDL_SWSURFACE);

	screenBuffer = malloc(64*32);
	gfx_cls();
}

void gfx_cls()
{
	SDL_FillRect(screen, NULL, 0);
	memset(screenBuffer, 0, 64*32);
}

int gfx_get_pixel(int x, int y)
{
	return screenBuffer[y*64 + x];
}

int gfx_draw_pixel_on(int x, int y)
{
	int colour = 1;

	x &= 63;
	y &= 31;

	if(gfx_get_pixel(x, y))
	{
		colour = 0;
	}

	screenBuffer[y*64 + x] = colour;

	return colour != 1;
}

int gfx_draw(int x, int y, unsigned char *mem, int count)
{
	int shouldFlip = 0;
	for(int j = 0; j < count; j++)
	{
		unsigned char byte = mem[j];
		for(int i = 1; i <= 8; i++)
		{
			int pixelOn = byte & (1 << (8-i));
			if(pixelOn)
			{
				shouldFlip |= gfx_draw_pixel_on(x+i, y + j);
			}
		}
	}
	return shouldFlip;
}


void gfx_flip()
{
	SDL_Rect brush = { 0, 0, 10, 10 };
	for(int y = 0; y < 32; y++)
	{
		for(int x = 0; x < 64; x++)
		{
			brush.x = x*10;
			brush.y = y*10;
			SDL_FillRect(screen, &brush, screenBuffer[y*64 + x] * 0xffffffff);
		}
	}
	SDL_Flip(screen);
}