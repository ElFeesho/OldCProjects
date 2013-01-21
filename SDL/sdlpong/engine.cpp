#include <iostream>
#include "SDLW.hpp"
#include "engine.hpp"
#include "ent.hpp"

Engine::Engine() : keys_down(0)
{

}

Engine::~Engine()
{
	entity_list.clear();
}

bool Engine::update()
{
	SDLW::draw_black();
	std::vector<Ent*>::iterator it;
	for(it = entity_list.begin();it!=entity_list.end();)
	{
		if(!((*it)->update(keys_down)))
		{
			delete((*it));
			it = entity_list.erase(it);
			continue;
		}
		else
		{
			(*it)->draw();
		}
		it++;
	}
	SDLW::draw_surf(Engine::get_buffer(),Engine::get_screen());
	SDL_Flip(Engine::get_screen());
	return true;
}

void Engine::add_ent(Ent *entity)
{
	entity->set_eng(this);
	entity_list.push_back(entity);
}

void Engine::remove_ent(Ent *entity)
{
	std::vector<Ent*>::iterator it;
	for(it = entity_list.begin();it!=entity_list.end();it++)
	{
		if((*it) == entity)
		{
			it = entity_list.erase(it);
			return;
		}
	}
}

void Engine::set_keys(int keys)
{
	keys_down = keys;
}

int Engine::get_keys()
{
	return keys_down;
}

Ent *Engine::check_collide(Ent *obj, std::string name)
{
	std::vector<Ent*>::iterator it;
	for(it = entity_list.begin();it!=entity_list.end();it++)
	{
		if((*it) == obj)
		{
			continue;
		}
		if((*it)->get_name() == name)
		{
			int x,xo,y,yo;
			x = (int)obj->get_x();
			xo = (int)(x+obj->get_w());
			y = (int)obj->get_y();
			yo = (int)(y + obj->get_h());
			if((xo>(*it)->get_x() && xo<(*it)->get_x()+(*it)->get_w()) && (yo>(*it)->get_y() && yo<(*it)->get_y()+(*it)->get_h()))
				return (*it);
			if((xo>(*it)->get_x() && xo<(*it)->get_x()+(*it)->get_w()) && (y<(*it)->get_y()+(*it)->get_h() && y>(*it)->get_y()))
				return (*it);
			if((x<(*it)->get_x()+(*it)->get_w() && x>(*it)->get_x()) && (yo>(*it)->get_y() && yo<(*it)->get_y()+(*it)->get_h()))
				return (*it);
			if((x<(*it)->get_x()+(*it)->get_w() && x>(*it)->get_x()) && (y<(*it)->get_y()+(*it)->get_h() && y>(*it)->get_y()))
				return (*it);
		}
	}
	return NULL;
}

Ent *Engine::get_ent_by_name(std::string name)
{
	std::vector<Ent*>::iterator it;
	for(it = entity_list.begin();it!=entity_list.end();it++)
	{
		if((*it)->get_name() == name)
		{
			return (*it);
		}
	}
	return NULL;
}

SDL_Surface *Engine::screen = NULL;
SDL_Surface *Engine::buffer = NULL;

void Engine::set_screen(SDL_Surface *nscreen)
{
	if(!nscreen)
	{
		std::cout << "What the? - the screen is NULL!" << std::endl;
		exit(-1);
	}
	Engine::screen = nscreen;
	if(!buffer)
		Engine::buffer = SDL_CreateRGBSurface(SDL_SWSURFACE,screen->w, screen->h,32,0,0,0,0);
	else
	{
		SDL_FreeSurface(buffer);
		Engine::buffer = SDL_CreateRGBSurface(SDL_SWSURFACE,screen->w, screen->h,32,0,0,0,0);
	}
}

SDL_Surface *Engine::get_screen()
{
	return Engine::screen;
}

SDL_Surface *Engine::get_buffer()
{
	return Engine::buffer;
}

