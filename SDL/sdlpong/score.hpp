#ifndef __SCORE_HPP__
#define __SCORE_HPP__

#include "SDL/SDL.h"
#include "ent.hpp"

class Score : public Ent
{
	public:
		Score();
		~Score();
		void draw();
	private:
		SDL_Surface *score_img;
};

#endif
