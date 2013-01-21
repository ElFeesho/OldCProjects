#ifndef __OMGISSILE_HPP__
#define __OMGISSILE_HPP__

#include <entity.hpp>
#include <sprite.hpp>

class OMGissile : public Entity
{
public:
	OMGissile(double x, double y, double xspeed, Scene *parent = NULL);
	~OMGissile();
	bool update(Uint8 *keys);
private:
	double xspeed;
	static Sprite *omgissile;
};

#endif
