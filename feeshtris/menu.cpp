#include <swordfeesh.hpp>
#include "menu.hpp"
#include "bubble.hpp"
#include "menusel.hpp"
#include "game.hpp"

using namespace SwordFeesh;

Menu::Menu() : title(new Image("images/title.png")), menu_txt(new Image("images/menutxt.png")), bubbles(new EntityList()), menu_sel(new MenuSel(275, 385)), menuopt(0)
{
	int w, h;
	Engine::get_inst()->get_viewport(&w, &h);

	for(int i = 0;i<4;i++)
	{
		for(int j = 50; j<w;j+=200)
		{
			bubbles->add(new Bubble(j, h+i*h/8, 1000*0.9*i));
		}
	}

	menu_sel = new MenuSel();
	menu_sel->set_pos((w-menu_sel->W())/2, h/8*7-menu_txt->H()+menuopt*47-10,(w-menu_sel->W())/2, h/8*7-menu_txt->H()+menuopt*47-10);
	bubbles->add(menu_sel);
}

Menu::~Menu()
{

}

bool Menu::update(Uint8 *keys)
{
	int w, h;
	Engine::get_inst()->get_viewport(&w, &h);

	glLoadIdentity();
	glBegin(GL_QUADS);
		glColor3f(0,0.8,1);
		glVertex2i(0,0);
		glVertex2i(w,0);
		glColor3f(0,0.5,1);
		glVertex2i(w,h);
		glVertex2i(0,h);
	glEnd();

	if(keys[SDLK_DOWN])
	{
		keys[SDLK_DOWN] = 0;
		menuopt++;
		if(menuopt>2)
			menuopt = 0;
		menu_sel->set_pos((w-menu_sel->W())/2, h/8*7-menu_txt->H()+menuopt*47-10);
	}

	if(keys[SDLK_UP])
	{
		keys[SDLK_UP] = 0;
		menuopt--;
		if(menuopt<0)
			menuopt = 2;
		menu_sel->set_pos((w-menu_sel->W())/2, h/8*7-menu_txt->H()+menuopt*47-10);
	}

	if(keys[SDLK_RETURN])
	{
		keys[SDLK_RETURN] = 0;
		switch(menuopt)
		{
			case 0:
				Engine::get_inst()->set_scene(new SwipeTransition(new Menu(*this), new Game(), 5000));
				break;
			case 1:
				//options
				break;
			case 2:
				Engine::get_inst()->set_scene(new FadeTransition(new Menu(*this), NULL, 500));
				break;
		}
	}

	title->draw((w-title->W())/2, h/8);
	menu_txt->draw((w-menu_txt->W())/2,h/8*7-menu_txt->H());
	bubbles->update();
	return true;
}
