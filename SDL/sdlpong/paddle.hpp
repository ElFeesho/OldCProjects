#ifndef __PADDLE__HPP__
#define __PADDLE__HPP__

#include "ent.hpp"

class Paddle : public Ent
{
   public:
      Paddle(const char *id, int player, double x, double y);
      ~Paddle();
		virtual bool update(int keys);
	private:
		int pnum;
};

#endif
