#include <iostream>
#include "game.hpp"
#include "roflcoptah.hpp"
#include "main.hpp"

using namespace std;

Game::Game() : Scene()
{
	SDL_mutexP(data_lock_coptah);
	SDL_mutexV(data_lock_coptah);

	ent_list = get_ent_list();
	bg = new BG();
	coptah = new Roflcoptah(this);
}

Game::~Game()
{
	Mix_HaltChannel(-1);
	Mix_FreeChunk(dz);
}

bool Game::update(Uint8 *keys)
{
	bg->update(keys);
	coptah->update(keys);

	ent_list->update();

	return true;
}

void Game::load_data()
{
//	dz = Mix_LoadWAV("music/dz.ogg");
//	if(!dz)
//		cout << "Failed loading sound " << Mix_GetError() << endl;
}

Mix_Chunk *Game::dz = NULL;
BG *Game::bg = NULL;
