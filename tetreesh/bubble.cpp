#include <cmath>
#include "bubble.hpp"

Bubble::Bubble()
{
	if(!bub_img)
		bub_img = new Image("images/bubble.png");
}

Bubble::Bubble(int x, int y, double i):x(x), y(y), a(0.5), scale(1.0), i(i), orig_x(x)
{
	if(!bub_img)
		bub_img = new Image("images/bubble.png");
}

Bubble::~Bubble()
{

}

bool Bubble::update(Uint8 *keys)
{
	bub_img->alpha(a);
	scale = 1.0*((600-y)/600);
	bub_img->scale(scale);
	i+=0.05;
	x = orig_x+cos(i)*30;
	bub_img->draw(x, y);
	y-=0.9+((600-y)/600);
	if(y<-90)
		y=600;
	return true;
}

Image *Bubble::bub_img = NULL;
