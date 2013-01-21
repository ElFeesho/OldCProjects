#include <SDL/SDL.h>
#include <GL/gl.h>
#include "transition.hpp"
#include "engine.hpp"
#include "scene.hpp"

using namespace SwordFeesh;

Transition::Transition()
{

}

Transition::Transition(Scene *one, Scene *two, long trans_time) : first(one), second(two), start_time(Engine::get_inst()->get_ticks()), trans_time(trans_time)
{

}

bool Transition::update(Uint8 *keys)
{
	if(Engine::get_inst()->get_ticks()<start_time+trans_time/2)
		first->update(keys);
	else
		second->update(keys);

	if(Engine::get_inst()->get_ticks()>start_time+trans_time)
	{
		Engine::get_inst()->set_scene(second);
		return false;
	}

	int w, h;
	Engine::get_inst()->get_viewport(&w, &h);
	glLoadIdentity();
	glEnable(GL_BLEND);
	if(Engine::get_inst()->get_ticks()<start_time+trans_time/2)
		glColor4d(0.0,0.0,0.0,(Engine::get_inst()->get_ticks()-start_time)/(trans_time/2.0));
	else
		glColor4d(0.0,0.0,0.0,1.0-(Engine::get_inst()->get_ticks()-start_time-(trans_time/2.0))/(trans_time/2.0));
	glBegin(GL_QUADS);
		glVertex2i(0,0);
		glVertex2i(w,0);
		glVertex2i(w,h);
		glVertex2i(0,h);
	glEnd();
	glDisable(GL_BLEND);


	return true;
}

