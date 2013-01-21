#ifndef __SPRITE_HPP__
#define __SPRITE_HPP__

#include "engine.hpp"
#include "image.hpp"

class Sprite
{
public:
	Sprite(const char *filename);
	~Sprite();
	void draw(int x, int y);

	Uint32 W();
	Uint32 H();

	Image *get_sheet();
private:
	Image *sheet;
	Uint32 w, h;
	Uint32 ltime;
	Uint16 frames;
	Uint16 cframe;
	bool const_time;
	Uint32 *delays;
};

#endif
