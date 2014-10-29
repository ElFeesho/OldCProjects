#include <SDL/SDL.h>

static int keymap[] = {
	SDLK_1, SDLK_2, SDLK_3, SDLK_4,
	SDLK_q, SDLK_w, SDLK_e, SDLK_r,
	SDLK_a, SDLK_s, SDLK_d, SDLK_f,
	SDLK_z, SDLK_x, SDLK_c, SDLK_v
};

int input_readkey()
{
	int result = -1;
	SDL_Event ev = { 0 };

	while(result < 0)
	{
		SDL_WaitEvent(&ev);
		if(ev.type == SDL_KEYDOWN)
		{
			int sym = ev.key.keysym.sym;
			for(int i = 0; i < 16; i++)
			{
				if(sym == keymap[i])
				{
					result = i;
					break;
				}
			}
		}
		else if(ev.type == SDL_QUIT)
		{
			exit(-1);
		}
	}

	return result;
}

int input_keydown(int key)
{
	Uint8 *keys = SDL_GetKeyState(NULL);

	return keys[keymap[key]];
}
