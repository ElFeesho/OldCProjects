#include <iostream>
#include <SDL/SDL.h>
#include <cmath>
#include "main.hpp"
#include "roflcoptah.hpp"
#include "wtfbomb.hpp"
#include "game.hpp"
#define MAX_SPEED 2.5

using namespace std;

Roflcoptah::Roflcoptah(Scene *parent) : Entity(parent), rot(0), xspeed(0), yspeed(0), tfactor(1.0)
{
	X(209);
	Y(150);
	SDL_mutexP(data_lock_coptah); // Need to make sure the sprite was loaded!
	SDL_mutexV(data_lock_coptah);
	W(coptah->W());
	H(coptah->H());
}

Roflcoptah::~Roflcoptah()
{
	delete coptah;
}

bool Roflcoptah::update(Uint8 *keys)
{
	static long ltick = 0;
	if(ltick == 0)
		ltick = Engine::get_inst()->get_ticks();
	if(keys[SDLK_a])
	{
		if(xspeed>0)
		{
			xspeed *= 0.9;//*tfactor;
			xspeed -= 0.1;//*tfactor;
		}
		else
		{
			if(xspeed>-MAX_SPEED)
			{
				xspeed *= 1.1;//*tfactor;
				xspeed -= 0.1;//*tfactor;
			}
			else
				xspeed = -MAX_SPEED;
		}
	}
	else if(keys[SDLK_d])
	{
		if(xspeed<0)
		{
			xspeed*=0.9;//*tfactor;
			xspeed+=0.1;//*tfactor;
		}
		else
		{
			if(xspeed<MAX_SPEED)
			{
				xspeed *= 1.1;//*tfactor;
				xspeed += 0.1;//*tfactor;
			}
			else
				xspeed = MAX_SPEED;
		}
	}
	else
	{
		xspeed*=0.99;
	}

	if(keys[SDLK_w])
	{
		if(yspeed>0)
		{
			yspeed *= 0.9;//*tfactor;
			yspeed -= 0.1;//*tfactor;
		}
		else
		{
			if(yspeed>-MAX_SPEED)
			{
				yspeed *= 1.1;//*tfactor;
				yspeed -= 0.1;//*tfactor;
			}
			else
				yspeed = -MAX_SPEED;
		}
	}
	else if(keys[SDLK_s])
	{
		if(yspeed<0)
		{
			yspeed*=0.9;//*tfactor;
			yspeed+=0.1;//*tfactor;
		}
		else
		{
			if(yspeed<MAX_SPEED)
			{
				yspeed *= 1.1;//*tfactor;
				yspeed += 0.1;//*tfactor;
			}
			else
				yspeed = MAX_SPEED;
		}
	}
	else
	{
		yspeed*=0.99;
	}

	if(keys[SDLK_SPACE])
	{
		keys[SDLK_SPACE] = 0;
		get_parent()->get_ent_list()->add(new WTFBomb(X()+W()/2,Y()+H(),1.2+xspeed*1.2,yspeed));
	}


	rot = 10;
	coptah->rotation(rot);
	X(X()+xspeed);
	Y(Y()+yspeed);

	coptah->draw(SX(Game::bg), SY(Game::bg));
	long tdiff = Engine::get_inst()->get_ticks()-ltick;
	tfactor = 1.0f+tdiff/16.0f;
	ltick = Engine::get_inst()->get_ticks();
	return true;
}

void Roflcoptah::load_data()
{
	coptah = new Sprite("sprites/rofl.spr");
}

Sprite *Roflcoptah::coptah = NULL;
