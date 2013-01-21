#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>

#include "res.h"

SDL_mutex *res_mutex = NULL;

static SDL_Surface *kit = NULL;
static Mix_Chunk *samples[10] = { NULL };

int res_load_resources(void *data)
{
	if(!res_mutex)
		res_mutex = SDL_CreateMutex();
	SDL_mutexP(res_mutex);
	int i = 0;
	samples[i++] = Mix_LoadWAV("sound/bass.wav");
	samples[i++] = Mix_LoadWAV("sound/cym.wav");
	samples[i++] = Mix_LoadWAV("sound/hihat.wav");
	samples[i++] = Mix_LoadWAV("sound/hihat2.wav");
	samples[i++] = Mix_LoadWAV("sound/snare.wav");
	samples[i++] = Mix_LoadWAV("sound/cra.wav");
	samples[i++] = Mix_LoadWAV("sound/hi.wav");
	samples[i++] = Mix_LoadWAV("sound/mid.wav");
	samples[i++] = Mix_LoadWAV("sound/low.wav");
	samples[i++] = Mix_LoadWAV("sound/stick.wav");
	kit = IMG_Load("kit.png");
	SDL_mutexV(res_mutex);
	return 0;
}

void res_loaded()
{
	SDL_mutexP(res_mutex);
	SDL_mutexV(res_mutex);
	SDL_DestroyMutex(res_mutex);
}

void res_draw_kit(SDL_Surface *screen)
{
	static int alpha = 0;
	SDL_Rect pos = { 320-kit->w/2, 240-kit->h/2,kit->w,kit->h };
	if(alpha<255)
		SDL_SetAlpha(kit, SDL_SRCALPHA,alpha++);
	SDL_BlitSurface(kit,NULL,screen,&pos);
}

void res_play(drum_sounds sound, int channel)
{
	Mix_PlayChannel(channel, samples[sound], 0);
}
