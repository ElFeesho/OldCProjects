#include <iostream>
#include "engine.hpp"

#include "menu.hpp"
#include "game.hpp"

using namespace std;

Engine::Engine() : scene(NULL), nscene(NULL), quit(false)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	screen = SDL_SetVideoMode(640,480,32,SDL_SWSURFACE);

	if(Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024)<0)
	{
		cerr << "Couldn't Initialise Audio" << endl;
	}

	inst = this;
}

Engine::~Engine()
{
	SDL_Quit();
}

bool Engine::update()
{
	if(nscene!=NULL)
	{
		if(scene)
		{
			delete scene;
		}
		scene = nscene;
		nscene = NULL;
	}
	ticks = SDL_GetTicks();

	Uint8 *keys = SDL_GetKeyState(NULL);

	scene->update(keys);

	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		if(ev.type == SDL_QUIT)
			return false;
	}

	SDL_Flip(screen);
	/* Lock at 60fps - Believe me I don't like it either */
	//if(SDL_GetTicks()-ticks<16)
		//SDL_Delay(SDL_GetTicks()-ticks);

	return !quit;
}

void Engine::set_scene(Scene *scene)
{
	if(scene == NULL)
		return;
	nscene = scene;
}

Engine *Engine::get_inst()
{
	return inst;
}

Uint32 Engine::get_ticks()
{
	return ticks;
}

void Engine::shutdown()
{
	quit = true;
}

void Engine::clear_screen(int r, int g, int b)
{
	SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,r,g,b));
}

void Engine::get_viewport(int *w, int *h)
{
	*w = screen->w;
	*h = screen->h;
}

Engine *Engine::inst = NULL;
Uint32 Engine::ticks = 0;
