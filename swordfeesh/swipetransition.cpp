#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "swipetransition.hpp"
#include "engine.hpp"
#include "scene.hpp"

using namespace SwordFeesh;

SwipeTransition::SwipeTransition()
{

}

SwipeTransition::SwipeTransition(Scene *one, Scene *two, long trans_time) : Transition(one,two,trans_time)
{

}

SwipeTransition::~SwipeTransition()
{

}

bool SwipeTransition::update(Uint8 *keys)
{
	Uint8 blanks[1024] = { 0 };

	if(Engine::get_inst()->get_ticks()>start_time+trans_time)
	{
		second->update(blanks);
		Engine::get_inst()->set_scene(second);
		delete first;
		return false;
	}

	first->update(blanks);

	int w, h;
	Engine::get_inst()->get_viewport(&w, &h);
	glLoadIdentity();
	glEnable(GL_STENCIL_TEST);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_NEVER, 0x0, 0x0);
	glStencilOp(GL_INCR, GL_INCR, GL_INCR);
	glColor3f(1.0f, 1.0f, 1.0f);
	double perc = ((double)(Engine::get_inst()->get_ticks()-start_time)/trans_time);
	for(int i = 0; i<(w+(w%32))/32;i++)
	{
		for(int j = 0; j<(h+(h%32))/32;j++)
		{
			glRectf(i*32+16-(32.0*perc)/2,j*32+16-(32.0*perc)/2,i*32+16+(32.0*perc)/2, j*32+16+(32.0*perc)/2);
		}	
	}	

	glStencilFunc(GL_EQUAL,0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	
	second->update(blanks);
	glDisable(GL_STENCIL_TEST);
	
	return true;
}

