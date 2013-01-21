#ifndef __FADE_TRANSITION_HPP__
#define __FADE_TRANSITION_HPP__

#include "transition.hpp"

namespace SwordFeesh
{
	class FadeTransition : public Transition
	{
	public:
		FadeTransition();
		FadeTransition(Scene *one, Scene *two, long trans_time);
		~FadeTransition();
		bool update(Uint8 *keys);
	};
}

#endif
