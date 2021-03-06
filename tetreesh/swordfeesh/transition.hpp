#ifndef __TRANSITION_HPP__
#define __TRANSITION_HPP__

#include "scene.hpp"

namespace SwordFeesh
{
	class Transition : public Scene
	{
	public:
		Transition();
		Transition(Scene *one, Scene *two, long trans_time);
		bool update(Uint8 *keys);
	private:
		Scene *first;
		Scene *second;
		long trans_time;
		long start_time;
	};
}

#endif
