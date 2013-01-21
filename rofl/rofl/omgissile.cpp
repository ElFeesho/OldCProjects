#include "omgissile.hpp"

OMGissile::OMGissile(double x, double y, double xspeed, Scene *parent) : Entity(x,y,parent), xspeed(xspeed)
{
	if(!omgissile)
		omgissile = new Sprite("sprites/omgissile.spr");
}
OMGissile::~OMGissile()
{

}

bool OMGissile::update(Uint8 *keys)
{
	X(X()+xspeed);
	omgissile->draw(X(),Y());
}

Sprite *OMGissile::omgissile = NULL;
