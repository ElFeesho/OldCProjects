#include "menu.hpp"

Menu::Menu()
{
	title = new Image("rsrcs/menu.png");
	pf = new ParticleFountain("rsrcs/particle.png",100, 1000, 100, 60, 0, 1, 0);
}

Menu::~Menu()
{
	delete title;
}

bool Menu::update(Uint8 *keys)
{
	static double x = 0;
	int w, h;
	Engine::get_inst()->get_viewport(&w,&h);
	
	title->draw((w-title->W())/2,10);
	
	pf->update();
	pf->X(x);
	x+=5;
	if(x>w)
		x = 0;
	return true;
}