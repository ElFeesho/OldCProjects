#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include <cmath>
#include "main.hpp"
#include "roflcoptah.hpp"
#include "game.hpp"
#define MAX_SPEED 1.2

using namespace std;

Roflcoptah::Roflcoptah() : Entity(), rot(0), xspeed(0), yspeed(0)
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
	if(keys[SDLK_LEFT])
	{
		if(xspeed>0)
		{
			xspeed *= 0.99;
			xspeed -= 0.01;
		}
		else
		{
			if(xspeed>-MAX_SPEED)
			{
				xspeed *= 1.01;
				xspeed -= 0.01;
			}
			else
				xspeed = -MAX_SPEED;
		}
	}
	else if(keys[SDLK_RIGHT])
	{
		if(xspeed<0)
		{
			xspeed*=0.99;
			xspeed+=0.01;
		}
		else
		{
			if(xspeed<MAX_SPEED)
			{
				xspeed *= 1.01;
				xspeed += 0.01;
			}
			else
				xspeed = MAX_SPEED;
		}	
	}
	else
	{
		xspeed*=0.99;
	}

	if(keys[SDLK_UP])
	{
		if(yspeed>0)
		{
			yspeed *= 0.99;
			yspeed -= 0.01;
		}
		else
		{
			if(yspeed>-MAX_SPEED)
			{
				yspeed *= 1.01;
				yspeed -= 0.01;
			}
			else
				yspeed = -MAX_SPEED;
		}
	}
	else if(keys[SDLK_DOWN])
	{
		if(yspeed<0)
		{
			yspeed*=0.99;
			yspeed+=0.01;
		}
		else
		{
			if(yspeed<MAX_SPEED)
			{
				yspeed *= 1.01;
				yspeed += 0.01;
			}
			else
				yspeed = MAX_SPEED;
		}	
	}
	else
	{
		yspeed*=0.99;
	}

	rot = (-25.0*(xspeed/MAX_SPEED));

	long tdiff = Engine::get_inst()->get_ticks()-ltick;
	X(X()+(xspeed*tdiff/(1000.0/60.0)));
	Y(Y()+(yspeed*tdiff/(1000.0/60.0)));
	
	coptah->draw(SX(Game::bg), SY(Game::bg));
	ltick = Engine::get_inst()->get_ticks();
	return true;
}

void Roflcoptah::load_data()
{
	coptah = new Sprite("sprites/rofl.spr");
}

Sprite *Roflcoptah::coptah = NULL;
