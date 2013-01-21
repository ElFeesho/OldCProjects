#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <GL/gl.h>

/*
	This class wraps up all drawing functions of an SDL_Surface
*/
namespace SwordFeesh
{
	class Image
	{
	public:
		Image();
		Image(const char *filename);
		~Image();
		void draw();
		void draw(int x, int y);
		void draw(int x, int y, int w, int h);
		void draw(int x, int y, double cx, double cy, double cw, double ch);
		void draw(int x, int y, int w, int h, double cx, double cy, double cw, double ch);

		int W();
		int H();

		void set_tint(double r, double g, double b);

		void rotation(double r);

		void alpha(double a);
		void scale(double s);
	private:
		void predraw(int x, int y);
		void enddraw();
		GLuint image;
		double _w, _h;
		double rot;
		double _r, _g, _b;
		double a;
		double _scale;
	};
}

#endif
