#ifndef __BUBBLE_HPP__
#define __BUBBLE_HPP__

#include <swordfeesh.hpp>

using namespace SwordFeesh;

class Bubble : public Entity
{
public:
	Bubble();
	Bubble(int x,int y, double i);
	~Bubble();
	bool update(Uint8 *keys);
private:
	double x;
	double y;
	double a;
	double scale;
	double i;
	double orig_x;
	static Image *bub_img;
};

#endif
