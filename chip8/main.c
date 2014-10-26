#include <SDL/SDL.h>
#include <stdio.h>
#include "gfx.h"

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
	while(parse_op() && !quit)
	{
		SDL_Delay(1000/10);
		SDL_Event ev = { 0 };
		while(SDL_PollEvent(&ev))
		{
				if(ev.type == SDL_QUIT)
				{
					quit = 1;
				}
		}
		SDL_Flip(SDL_GetVideoSurface());

	}
	
	return 0;
}
