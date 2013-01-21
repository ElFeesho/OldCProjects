#ifndef __ROFLCOPTAH_HPP__
#define __ROFLCOPTAH_HPP__

#include <SDL/SDL.h>

#include <entity.hpp>
#include <sprite.hpp>

class Roflcoptah : public Entity
{
public:
	Roflcoptah(Scene *parent = NULL);
	~Roflcoptah();

	bool update(Uint8 *keys);
	static void load_data();
private:
	static Sprite *coptah;
	double rot;
	double xspeed;
	double yspeed;
	double tfactor;
};

#endif
