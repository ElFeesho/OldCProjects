#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

#include <SDL/SDL.h>

class Entity
{
public:
	Entity();
	virtual ~Entity();

	virtual bool update(Uint8 *keys) = 0;

	void X(double x);
	double X();
	
	void Y(double y);
	double Y();

	void W(double w);
	double W();

	void H(double h);
	double H();

	double SX(Entity *container);
	double SY(Entity *container);

private:	
	double x, y;
	double w, h;
};

#endif
