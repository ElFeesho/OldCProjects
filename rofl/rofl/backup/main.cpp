#include <iostream>
#include "engine.hpp"
#include "menu.hpp"
#include "roflcoptah.hpp"
#include "game.hpp"

SDL_mutex *data_lock_coptah = NULL;

int load_thread(void *data)
{
	// Load Game resources in the background
	SDL_mutexP(data_lock_coptah);
	Roflcoptah::load_data();
	Game::load_data();
	SDL_mutexV(data_lock_coptah);
}

int main(int argc, char **argv)
{
	Engine *game_eng = new Engine();

	// Load resources in the background :D
	data_lock_coptah = SDL_CreateMutex();
	SDL_CreateThread(load_thread,NULL); 

	game_eng->set_scene(new Menu());

	while(game_eng->update());

	delete game_eng;

	return 0;
}
