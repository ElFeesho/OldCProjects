#include <SDL/SDL.h>

SDL_Surface *average_surfaces(SDL_Surface **surfaces, int count)
{
	SDL_Surface *dst = SDL_CreateRGBSurface(SDL_SWSURFACE,surfaces[0]->w,surfaces[0]->h,32,0,0,0,0);
	int row, col, csurf;
	unsigned char *pixels = (unsigned char*)dst->pixels;
	for(row = 0;row<dst->h;row++)
	{
		for(col=0;col<dst->w*4;col++)
		{
			int total = 0;
			for(csurf = 0;csurf<count;csurf++)
			{
				total += *(unsigned char*)(surfaces[csurf]->pixels+row*dst->w*4+col);
			}
			total /= count;
			pixels[row*dst->w*4+col] = (unsigned char)total;
		}
	}
	return dst;
}
