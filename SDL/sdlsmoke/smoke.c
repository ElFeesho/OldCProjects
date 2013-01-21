#include <SDL/SDL.h>
#include <stdlib.h>
#include <time.h>

SDL_Surface *smoke_generate_layer(int lnum)
{
	SDL_Surface *dst = SDL_CreateRGBSurface(SDL_SWSURFACE,lnum*5,lnum*10,32,0,0,0,0);
	int i = 0;
	int j = 0;
	unsigned char *pixels = (unsigned char*)dst->pixels;
	for(i = 0; i<lnum*5;i++)
	{
		for(j = 0; j<lnum*5;j++)
		{	
			unsigned char val = rand()%255;
			pixels[i*((lnum*5)*4)+j*4] = val;
			pixels[i*((lnum*5)*4)+j*4+1] = val;
			pixels[i*((lnum*5)*4)+j*4+2] = val;
		}
	}
	return dst;
}
