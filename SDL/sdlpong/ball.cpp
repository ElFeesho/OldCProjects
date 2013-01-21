#include <iostream>
#include <typeinfo>
#include "ball.hpp"
#include "engine.hpp"
#include "score.hpp"

Ball::Ball(const char *id, double x, double y, double vel) : Ent(id, vel, vel, x, y,20,20)
{
	
}

Ball::~Ball()
{

}

bool Ball::update(int keys)
{
	//First of all keys, are not used!
	set_x(get_x()+get_xv()*(get_time()/10.0));
	set_y(get_y()+get_yv()*(get_time()/10.0));
	
	if(get_y()>Engine::get_screen()->h-get_h())
	{
		set_y(Engine::get_screen()->h-get_h());
		if(abs((int)get_yv())<5)
			set_yv(get_yv()*-1.02);	
		else
			set_yv(get_yv()*-1.0);
	}
	
	if(get_y()<0)
	{
		set_y(0);
		if(abs((int)get_yv())<5)
			set_yv(get_yv()*-1.02);	
		else
			set_yv(get_yv()*-1.0);	
	}
	
	if(get_x()>Engine::get_screen()->w+100)
	{
		set_x(Engine::get_screen()->w/2-10);
		set_y(Engine::get_screen()->h/2-10);
		set_xv(-2.0);
		((Engine*)get_engine())->add_ent(new Score);
	}
	
	if(get_x()<-100)
	{
		set_x(Engine::get_screen()->w/2-10);
		set_y(Engine::get_screen()->h/2-10);
		set_xv(2.0);
		((Engine*)get_engine())->add_ent(new Score);
	}
	Ent *paddle1 = ((Engine*)get_engine())->check_collide(this,"paddle1");
	Ent *paddle2 = ((Engine*)get_engine())->check_collide(this,"paddle2");
	if(paddle1)
	{
		set_x(paddle1->get_x()+paddle1->get_w());
		set_xv(get_xv()*-1.01);
	}
	if(paddle2)
	{
		set_x(paddle2->get_x()-get_w());
		set_xv(get_xv()*-1.01);
	}
	
	update_time();
	return true;
}
