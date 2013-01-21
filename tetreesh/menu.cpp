#include <swordfeesh.hpp>
#include "menu.hpp"
#include "bubble.hpp"
#include "menusel.hpp"
#include "game.hpp"

using namespace SwordFeesh;

Menu::Menu()
{
	bg = new Image("images/menu.png");
	bubbles = new EntityList();
	for(int i = 0;i<8;i++)
	{
		bubbles->add(new Bubble(50, 600+i*40, 1000*0.9*i));
		bubbles->add(new Bubble(250, 600+i*40, -1000*0.9*i));
		bubbles->add(new Bubble(550, 600+i*40, 1000*0.9*i));
		bubbles->add(new Bubble(750, 600+i*40, -1000*0.9*i));
	}

	menu_sel = new MenuSel(275,385);
	menuopt = 0;
	bubbles->add(menu_sel);
}

Menu::~Menu()
{

}

bool Menu::update(Uint8 *keys)
{
	if(keys[SDLK_DOWN])
	{
		keys[SDLK_DOWN] = 0;
		menuopt++;
		if(menuopt>2)
			menuopt = 0;
		menu_sel->set_pos(275, 385+menuopt*47);
	}
	if(keys[SDLK_UP])
	{
		keys[SDLK_UP] = 0;
		menuopt--;
		if(menuopt<0)
			menuopt = 2;
		menu_sel->set_pos(275, 385+menuopt*47);
	}

	if(keys[SDLK_RETURN])
	{
		keys[SDLK_RETURN] = 0;
		switch(menuopt)
		{
			case 0:
				Engine::get_inst()->set_scene(new Transition(new Menu(*this), new Game(), 500));
				break;
			case 1:
				//options
				break;
			case 2:
				Engine::get_inst()->shutdown();
				break;
		}
	}

	bg->draw();
	bubbles->update();
	return true;
}
