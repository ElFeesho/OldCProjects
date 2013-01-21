#ifndef __SDLW_HPP__
#define __SDLW_HPP__

#include "SDL/SDL.h"
#include "engine.hpp"

class SDLW
{
	public:
		SDLW();
		~SDLW();
		static void draw_rect(int x, int y, int w, int h, char r, char g, char b);
		static void draw_surf(SDL_Surface *src, SDL_Surface *dest, int x, int y);
		static void draw_surf(SDL_Surface *src, SDL_Surface *dst);
		static void draw_black();
};

#endif
