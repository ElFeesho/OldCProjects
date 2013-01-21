#ifndef __ENGINE__HPP__
#define __ENGINE__HPP__

#include <vector>
#include <string>
#include "SDL/SDL.h"
#include "ent.hpp"

class Engine
{
	public:
		Engine();
		~Engine();
		bool update();
		void add_ent(Ent *entity);
		void remove_ent(Ent *entity);
		
		void set_keys(int keys);
		int get_keys();
		Ent *check_collide(Ent *obj, std::string name);
		Ent *get_ent_by_name(std::string name);
		
		static void set_screen(SDL_Surface *nscreen);
		static SDL_Surface *get_screen();
		static SDL_Surface *get_buffer();
		static SDL_Surface *screen;
		static SDL_Surface *buffer;
	private:
		std::vector<Ent*> entity_list;
		int keys_down;
};

#endif
