#include <SDL/SDL.h>

static SDL_Surface *screen = NULL;

void gfx_init();
void gfx_cls();
int gfx_draw_pixel_on();
int gfx_draw_pixel_off();
int gfx_draw(int x, int y, unsigned char *mem, int count);

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

int gfx_get_pixel(int x, int y)
{
	return ((unsigned char*)screen->pixels)[(y * screen->pitch + x * screen->format->BytesPerPixel)];
}

int gfx_draw_pixel_on(int x, int y)
{
	int colour = 0xffffffff;
	if(gfx_get_pixel(x*10, y*10))
	{
		colour = 0xff000000;
	}
	SDL_Rect pos = { .x = x*10, .y = y*10, .w = 10, .h = 10 };
	SDL_FillRect(screen, &pos, colour);
	return colour != 0xffffffff;
}

int gfx_draw(int x, int y, unsigned char *mem, int count)
{
	int i;
	int j;

	if(x > 64)
	{
		x = x % 64;
	}

	if(y > 32)
	{
		y = y % 32;
	}

	int shouldFlip = 0;
	for(j = 0; j < count; j++)
	{
		unsigned char byte = mem[j];
		for(i = 0; i < 8; i++)
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
