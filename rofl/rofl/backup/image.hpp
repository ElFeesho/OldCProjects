#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

/*
	This class wraps up all drawing functions of an SDL_Surface
*/

class Image
{
public:
	Image();
	Image(const char *filename);
	~Image();
	void draw();
	void draw(int x, int y);
	void draw(int x, int y, int w, int h);
	void draw(int x, int y, int cx, int cy, int cw, int ch);
	void draw(int x, int y, int w, int h, int cx, int cy, int cw, int ch);
	int w();
	int h();
private:
	SDL_Surface *image;
};

#endif
