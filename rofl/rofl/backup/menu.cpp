#include <iostream>
#include "menu.hpp"

#include "game.hpp"
#include "engine.hpp"
#include "sprite.hpp"

#include <SDL/SDL_image.h>

using namespace std;

Menu::Menu() : Scene()
{
	menu_img = new Image("images/menu.png");
	sel_item = new Image("images/sel.png");
	sel_choice = 0;
}

Menu::~Menu()
{
	delete menu_img;
	delete sel_item;
}

bool Menu::update(Uint8 *keys)
{
	static int lkey = 0;
	static Sprite *roflcoptah = NULL;

	if(!roflcoptah)
		roflcoptah = new Sprite("sprites/rofl.spr");

	menu_img->draw();

	roflcoptah->draw(209,150);

	if(keys[SDLK_RETURN])
	{
		if(sel_choice==0)
			Engine::get_inst()->set_scene(new Game());
		if(sel_choice==2)
			Engine::get_inst()->shutdown();
		return false;
	}

	if(keys[SDLK_UP] && lkey!=SDLK_UP)
	{
		lkey = SDLK_UP;
		if(sel_choice>0)
			sel_choice--;
	}
	else if(!keys[SDLK_UP] && lkey == SDLK_UP)
	{
		lkey = 0;
	}

	if(keys[SDLK_DOWN] && lkey!=SDLK_DOWN)
	{
		lkey = SDLK_DOWN;
		if(sel_choice<2)
			sel_choice++;
	}
	else if(!keys[SDLK_DOWN] && lkey == SDLK_DOWN)
		lkey = 0;

	sel_item->draw(214,320+(40*sel_choice));
	sel_item->draw(410,320+(40*sel_choice));

	return true;
}
