#include "score.hpp"
#include "engine.hpp"
#include "SDLW.hpp"
#include "SDL/SDL_image.h"
#include <iostream>

Score::Score() : Ent("score_inf", 0.0, 4.0, 320-128, -128, 256.0, 128.0), score_img(IMG_Load("score.png"))
{

}

Score::~Score()
{

}

void Score::draw()
{
	if(score_img!=NULL)
		SDLW::draw_surf(score_img, Engine::get_buffer(),(int)get_x(),(int)get_y());
}
