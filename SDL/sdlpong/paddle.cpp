#include "paddle.hpp"
#include "engine.hpp"
#include "SDLW.hpp"

Paddle::Paddle(const char *id, int player, double x, double y) : pnum(player) , Ent(id, 0, 0, x, y,20,70)
{

}

Paddle::~Paddle()
{

}

bool Paddle::update(int keys)
{
	if(pnum==1)
	{
		if(keys&1)
			set_yv(-2.0);
		if(keys&2)
			set_yv(2.0);
		if((keys&1)!= 1 && (keys&2)!=2)
			set_yv(0.0);
	}
	else if(pnum == 2)
	{
		if(keys&4)
			set_yv(-2.0);
		if(keys&8)
			set_yv(2.0);
		if((keys&4)!= 4 && (keys&8)!=8)
			set_yv(0.0);
	}
	
	set_y(get_y()+get_yv()*(get_time()/10.0));
	if(get_y()+get_h()>Engine::get_screen()->h)
	{
		set_y(Engine::get_screen()->h-get_h());
	}
	
	if(get_y()<0)
	{
		set_y(0);
	}
	
	update_time();
	return true;
}

