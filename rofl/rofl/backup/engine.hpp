#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <vector>

#include "scene.hpp"

using std::vector;

class Engine
{
public:
	Engine();
	~Engine();
	bool update();

	void set_scene(Scene *scene);

	static Engine *get_inst();
	static Uint32 get_ticks();
	void get_viewport(int *w, int *h);
	void shutdown();
	void clear_screen(int r = 0, int g = 0, int b = 0);

private:
	static Engine *inst;
	static Uint32 ticks;
	SDL_Surface *screen;
	Scene *nscene; // New scene
	Scene *scene; // The actual scene

	bool quit; // Shutdown sets this to true
};

#endif
