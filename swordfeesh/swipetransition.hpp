#ifndef __SWIPE_TRANSITION_HPP__
#define __SWIPE_TRANSITION_HPP__

#include "transition.hpp"

namespace SwordFeesh
{
	class SwipeTransition : public Transition
	{
	public:
		SwipeTransition();
		SwipeTransition(Scene *one, Scene *two, long trans_time);
		~SwipeTransition();
		bool update(Uint8 *keys);
	};
}

#endif
