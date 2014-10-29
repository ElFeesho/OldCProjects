#include <SDL/SDL.h>
#include <stdio.h>
#include "gfx.h"
#include "sound.h"
#include "ops.h"

static int NUM_OPS = 7;

int main(int argc, char ** argv)
{
	if(argc != 2 && argc != 3)
	{
		fprintf(stderr, "Usage:\n\t%s <GAME_FILE> [CYCLES]\n\n",argv[0]);
		return 1;
	}

	if(argc == 3)
	{
		sscanf(argv[2], "%d", &NUM_OPS);
	}

	chip8_cpu_t *cpu = create_cpu();

	if(load_game_from_file(cpu, argv[1]))
	{
		fprintf(stderr, "Couldn't load %s\n",argv[1]);
		return 1;
	}

	gfx_init();
	sound_init();
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
			parse_op(cpu);
		}

		gfx_flip();
		SDL_Delay(1000/60);
		decrement_timers(cpu);
	}

	destroy_cpu(cpu);

	return 0;
}
