#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

#include <stdio.h>

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	int x = 0, y = 0;
	int yspeed = 0;
	int xspeed = 0;
	int r = 200;

	SDL_Surface *screen = SDL_SetVideoMode(640, 480, 24, SDL_SWSURFACE);
	int quit = 0;
	while(!quit)
	{
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			switch(ev.type)
			{
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYDOWN:
					if(ev.key.keysym.sym == SDLK_LEFT)
						xspeed=-2;
					if(ev.key.keysym.sym == SDLK_RIGHT)
						xspeed=2;
					if(ev.key.keysym.sym == SDLK_UP)
						yspeed=-2;
					if(ev.key.keysym.sym == SDLK_DOWN)
						yspeed=2;
					break;
				case SDL_KEYUP:
					if(ev.key.keysym.sym == SDLK_LEFT)
						xspeed=0;
					if(ev.key.keysym.sym == SDLK_RIGHT)
						xspeed=0;
					if(ev.key.keysym.sym == SDLK_UP)
						yspeed=0;
					if(ev.key.keysym.sym == SDLK_DOWN)
						yspeed=0;
					break;
			}
		}
		x+=xspeed;
		y+=yspeed;
		if(((x-320)*(x-320))+((y-240)*(y-240))<(r*r))
			printf("Coliding?\n");
		SDL_FillRect(screen, NULL, 0);
		filledCircleRGBA(screen, 320, 240, r, 0xff,0xff,0xff,0xff);
		aalineRGBA(screen, x,y,x,y, 0xff,0x00,0x00,0xff);
		SDL_Flip(screen);
	}

	SDL_Quit();
	
	return 0;
}
