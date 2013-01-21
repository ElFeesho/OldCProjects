#include "SDL/SDL.h"
#include "SDLW.hpp"
#include "ent.hpp"
#include "engine.hpp"

#include <iostream>

Ent::Ent(const char *id, double x_vel, double y_vel, double x_pos, double y_pos, double w, double h) : xvel(x_vel), yvel(y_vel), xpos(x_pos), ypos(y_pos) , width(w), height(h), last_tick(0), current_tick(0)
{
	name = id;
	last_tick = SDL_GetTicks();
}

Ent::~Ent()
{

}

bool Ent::update(int keys)
{
	if(current_tick!=0)
	{
		xpos += xvel*(current_tick/10.0);
		ypos += yvel*(current_tick/10.0);
		if(ypos>Engine::get_screen()->h)
			return false;
	}
	update_time();
	return true;
}

void Ent::draw()
{
	SDLW::draw_rect((int)get_x(),(int)get_y(),(int)get_w(),(int)get_h(),255,255,255);
}

void Ent::set_x(double x)
{
	xpos = x;
}

void Ent::set_y(double y)
{
	ypos = y;
}

void Ent::set_xv(double xv)
{
	xvel = xv;
}

void Ent::set_yv(double yv)
{
	yvel = yv;
}

void Ent::set_w(double w)
{
	width = w;
}

void Ent::set_h(double h)
{
	height = h;
}

void Ent::set_eng(Engine *eng)
{
	engine = eng;
}

double Ent::get_x()
{
	return xpos;
}

double Ent::get_y()
{
	return ypos;
}

double Ent::get_xv()
{
	return xvel;
}

double Ent::get_yv()
{
	return yvel;
}

double Ent::get_w()
{
	return width;
}

double Ent::get_h()
{
	return height;
}

long Ent::get_time()
{
	return current_tick;
}

std::string Ent::get_name()
{
	return name;
}

Engine *Ent::get_engine()
{
	return engine;
}

void Ent::update_time()
{
	current_tick = SDL_GetTicks()-last_tick;
	last_tick = SDL_GetTicks();
}
