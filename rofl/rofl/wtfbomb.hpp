#ifndef __WTFBOMB_HPP__
#define __WTFBOMB_HPP__

#include <entity.hpp>
#include <sprite.hpp>

class WTFBomb : public Entity
{
public:
	WTFBomb(double x, double y, double xspeed, double yspeed, Scene *parent = NULL);
	~WTFBomb();
	bool update(Uint8 *keys);
private:
	double xspeed;
	double yspeed;
	static Sprite *wtfbomb;
};

#endif
