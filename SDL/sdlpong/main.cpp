#include <iostream>
#include "SDL/SDL.h"
#include "engine.hpp"
#include "ball.hpp"
#include "paddle.hpp"
#include "lpaddle.hpp"

int main(int argc, char **argv)
{
	int failure = 0;
	failure = SDL_Init(SDL_INIT_VIDEO);
	
	if(failure)
	{
		std::cout << "Failed to initialise SDL Video" << std::endl;
		return failure;
	}
	
	Engine *pong_engine = new Engine();
	Engine::set_screen(SDL_SetVideoMode(640,480,0,SDL_SWSURFACE));
	
	pong_engine->add_ent(new lpaddle((void*)pong_engine,"paddle1", 1, 10, Engine::get_screen()->h/2-35));
	pong_engine->add_ent(new lpaddle((void*)pong_engine,"paddle2", 2, Engine::get_screen()->w-30, Engine::get_screen()->h/2-35));
	pong_engine->add_ent(new Ball("ball", Engine::get_screen()->w/2-10,Engine::get_screen()->h/2-10,2.0));
	SDL_Event event;
	while(pong_engine->update())
	{
		while(SDL_PollEvent(&event)) 
		{
			switch(event.type)
			{
				case SDL_QUIT: 
					SDL_Quit();
					return 0;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_UP:
							pong_engine->set_keys(pong_engine->get_keys()|1);
							break;
						case SDLK_DOWN:
							pong_engine->set_keys(pong_engine->get_keys()|2);
							break;
						case SDLK_a:
							pong_engine->set_keys(pong_engine->get_keys()|4);
							break;
						case SDLK_z:
							pong_engine->set_keys(pong_engine->get_keys()|8);
							break;
						case SDLK_ESCAPE:
							SDL_Quit();
							return 0;
					}
					break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym)
					{
						case SDLK_UP:
							pong_engine->set_keys(pong_engine->get_keys()^1);
							break;
						case SDLK_DOWN:
							pong_engine->set_keys(pong_engine->get_keys()^2);
							break;
						case SDLK_a:
							pong_engine->set_keys(pong_engine->get_keys()^4);
							break;
						case SDLK_z:
							pong_engine->set_keys(pong_engine->get_keys()^8);
							break;
					}
					break;
			}
		}	
		SDL_Delay(10);
	}
	return 0;
}
