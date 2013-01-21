#ifndef __GAME_HPP__
#define __GAME_HPP__

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <vector>
#include "roflcoptah.hpp"
#include "bg.hpp"
#include "entitylist.hpp"
#include "scene.hpp"

using namespace std;

class Game : public Scene
{
public:
	Game();
	~Game();
	bool update(Uint8 *keys);
	static void load_data();
	static BG *bg;
private:
	static Mix_Chunk *dz;
	Roflcoptah *coptah;
	EntityList *ent_list;
};

#endif
