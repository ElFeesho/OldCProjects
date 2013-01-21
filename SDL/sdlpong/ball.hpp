#ifndef __BALL_HPP__
#define __BALL_HPP__

#include "ent.hpp"

class Ball : public Ent
{
	public:
		Ball(const char *id, double x, double y, double vel);
		~Ball();
		bool update(int keys);	
};

#endif
