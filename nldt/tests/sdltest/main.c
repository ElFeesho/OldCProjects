#include <SDL/SDL.h> 
#include <SDL/SDL_gfxPrimitives.h>
#include <math.h>

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen = SDL_SetVideoMode(500,400,24,SDL_SWSURFACE);
	atexit(SDL_Quit);
	
	SDL_Event ev;
	int i = 0;
	for(;;)
	{
		while(SDL_PollEvent(&ev))
		{
			switch(ev.type)
			{
				case SDL_QUIT:
					return 0;
					break;
			}
		}
		lineRGBA(screen, 200, 200,300+cos(i*3.142/180.0)*100,300+sin(i*3.142/180.0)*100, 255,0,255,255);
		i++;
		SDL_Flip(screen);
	}
	
	return 0;
}
