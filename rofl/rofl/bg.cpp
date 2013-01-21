#include "bg.hpp"

#include <engine.hpp>

BG::BG() : Entity()
{
	bg = new Image("images/bg.png");
}

BG::~BG()
{
	delete bg;
}

bool BG::update(Uint8 *keys)
{
	static int sw = 0, sh = 0, X = 0;
	if(!sw)
	{
		Engine::get_inst()->get_viewport(&sw, &sh);
	}
	X-=5;
	if(X<-sw)
		X=0;
	bg->draw(X,0,640,480);
	bg->draw(X+640,0,640,480);
	return true;
}

