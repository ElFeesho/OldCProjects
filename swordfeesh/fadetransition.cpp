#include <SDL/SDL.h>
#include <GL/gl.h>
#include "fadetransition.hpp"
#include "engine.hpp"
#include "scene.hpp"

using namespace SwordFeesh;

FadeTransition::FadeTransition()
{

}

FadeTransition::FadeTransition(Scene *one, Scene *two, long trans_time) : Transition(one,two,trans_time)
{

}

FadeTransition::~FadeTransition()
{

}

bool FadeTransition::update(Uint8 *keys)
{
	Uint8 blanks[1024] = { 0 };
	if(Engine::get_inst()->get_ticks()<start_time+trans_time/2)
		first->update(blanks);
	else
	{
		if(second==NULL)
		{
			Engine::get_inst()->shutdown();
			return false;
		}		
		second->update(blanks);
	}
	if(Engine::get_inst()->get_ticks()>start_time+trans_time)
	{
		second->update(blanks);
		Engine::get_inst()->set_scene(second);
		delete first;
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

