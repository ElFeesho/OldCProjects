#include <SDL/SDL.h>
#include <stdio.h>
#include "gfx.h"

extern int load_game(char *filename);
extern int parse_op();
extern void decrement_timers();

static int NUM_OPS = 11;

int main(int argc, char ** argv)
{
	if(argc!=2)
	{
		fprintf(stderr, "Usage:\n\t%s [GAME_FILE]\n\n",argv[0]);
		return 1;
	}

	if(load_game(argv[1]))
	{
		fprintf(stderr, "Couldn't load %s\n",argv[1]);
		return 1;
	}

	gfx_init();
	int quit = 0;
	while(!quit)
	{
		SDL_Event ev = { 0 };
		while(SDL_PollEvent(&ev))
		{
				if(ev.type == SDL_QUIT)
				{
					quit = 1;
				}
				else if (ev.type == SDL_KEYDOWN)
				{
					if(ev.key.keysym.sym == SDLK_p)
					{
						NUM_OPS++;
						printf("NUMOPS: %d\n", NUM_OPS);
					}
					if(ev.key.keysym.sym == SDLK_o)
					{
						NUM_OPS--;
						printf("NUMOPS: %d\n", NUM_OPS);
					}
				}
		}

		for(int i = 0; i < NUM_OPS; i++)
		{
			parse_op();
		}

		SDL_Flip(SDL_GetVideoSurface());
		SDL_Delay(1000/60);
		decrement_timers();
	}

	return 0;
}
