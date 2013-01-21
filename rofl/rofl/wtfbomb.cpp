#include <math.h>
#include "wtfbomb.hpp"

WTFBomb::WTFBomb(double x, double y, double xspeed, double yspeed, Scene *parent) : Entity(x,y,parent)
{
	if(!wtfbomb)
	{
		wtfbomb = new Sprite("sprites/wtfbomb.spr");
	}
	this->xspeed = xspeed;
	this->yspeed = yspeed;
}

WTFBomb::~WTFBomb()
{

}

bool WTFBomb::update(Uint8 *keys)
{
	if(yspeed<4.0)
	{
		yspeed+=0.05;
	}
	X(X()+xspeed);
	Y(Y()+yspeed);
	wtfbomb->rotation(atan2(yspeed,xspeed)*180/M_PI);
	wtfbomb->draw(X(), Y());
	if(Y()>480)
		return false;
	return true;
}

Sprite *WTFBomb::wtfbomb = NULL;
