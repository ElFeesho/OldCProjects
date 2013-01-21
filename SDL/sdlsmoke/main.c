#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern SDL_Surface *smoke_generate_layer(int lnum);

int main(int argc, char **argv)
{
	srand(time(NULL));
	
	SDL_Surface *screen;

	if(SDL_Init(SDL_INIT_VIDEO)<0)
	{
		fprintf(stderr, "Failed to initialise SDL Video\n");
		return -1;
	}
	
	screen = SDL_SetVideoMode(320,320,24,SDL_SWSURFACE);
	if(!screen)
	{
		fprintf(stderr, "Failed setting screen mode\n");
		SDL_Quit();
		return -1;
	}
	
	SDL_Surface *smoke[5];
	smoke[0] = smoke_generate_layer(1);
	smoke[1] = smoke_generate_layer(2);
	smoke[2] = smoke_generate_layer(3);
	smoke[3] = smoke_generate_layer(4);
	smoke[4] = smoke_generate_layer(5);
	
	int i = 0;
	smoke[0] = rotozoomSurface(smoke[0],0,320/5,1);
	smoke[1] = rotozoomSurface(smoke[1],0,320/10,1);
	smoke[2] = rotozoomSurface(smoke[2],0,320/15,1);
	smoke[3] = rotozoomSurface(smoke[3],0,320/20,1);
	smoke[4] = rotozoomSurface(smoke[4],0,320/25,1);
	
	
	SDL_Surface *avg_smoke = average_surfaces(smoke,5);
	
	//Blit the test to screen :D
	SDL_BlitSurface(avg_smoke, NULL, screen, NULL);
	
	
	/*
		Infinite loop time- loop until the user hits the close button
	*/
	
	for(;;)
	{
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			switch(ev.type)
			{
				case SDL_QUIT:
					SDL_Quit();
					return 0;
					break;
			}
		}
		SDL_Flip(screen);
	}
	return 0;
}
