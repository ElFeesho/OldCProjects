#ifndef __LOCATOR_HPP__
#define __LOCATOR_HPP__

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

#include "font.hpp"

class Locator
{
public:
	Locator();
	~Locator();
	void update();
private:
	Uint32 x, y;
	GLuint tex_id;
	SDL_Surface *arrow;
	Font *text;
	bool show;
	float alpha;
};

#endif

