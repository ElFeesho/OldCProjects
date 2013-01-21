#include "bg.hpp"
#include "engine.hpp"

BG::BG() : Entity()
{
	bg = new Image("images/bg.png");
}

BG::~BG()
{
	delete bg;
}

void BG::track(Entity *ent)
{
	static int sw = 0;
	static int sh = 0;
	if(!sw)
	{
		Engine::get_inst()->get_viewport(&sw,&sh);
	}
	double dx = ent->X()+ent->W()/2;
	double dy = ent->Y()+ent->H()/2;

	if(dx<sw/2)
		X(0);
	else if(dx>bg->w()-sw/2)
		X(sw-bg->w());
	else
		X(sw/2-dx);

	if(dy<sh/2)
		Y(0);
	else if(dy>bg->h()-sh/2)
		Y(sh-bg->h());
	else
		Y(sh/2-dy);
}

bool BG::update(Uint8 *keys)
{
	static int sw = 0, sh = 0;
	if(!sw)
	{
		Engine::get_inst()->get_viewport(&sw, &sh);
	}
	
	bg->draw(0,0, abs(X()), abs(Y()), sw, sh);
	return true;
}

