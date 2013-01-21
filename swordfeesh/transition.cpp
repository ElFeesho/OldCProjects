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

Transition::~Transition()
{

}

bool Transition::update(Uint8 *keys)
{
	Uint8 blanks[1024] = { 0 };
	if(Engine::get_inst()->get_ticks()<start_time+trans_time)
		first->update(blanks);
	
	if(Engine::get_inst()->get_ticks()>start_time+trans_time)
	{
		second->update(blanks);
		Engine::get_inst()->set_scene(second);
		delete first;
		return false;
	}

	return true;
}

