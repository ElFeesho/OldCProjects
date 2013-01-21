#include <iostream>

#include "image.hpp"

using namespace std;
using namespace SwordFeesh;

Image::Image():image(0)
{

}

Image::Image(const char *filename) : _w(0), _h(0), rot(0), _r(1.0), _g(1.0), _b(1.0), a(1.0), _scale(1.0)
{
	SDL_Surface *img = IMG_Load(filename);
	if(!img)
	{
		cout << "Failed loading image: " << filename << endl;
		cout << "Err Msg: " <<  IMG_GetError() << endl;
		return;
	}
	glGenTextures(1,&image);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, image);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	if(img->format->BitsPerPixel == 32)
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
	else
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, img->w, img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);

	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, image);
	_w = img->w;
	_h = img->h;
	SDL_FreeSurface(img);
}

Image::~Image()
{
	glDeleteTextures(1,&image);
}

void Image::draw()
{
	predraw(_w/2,_h/2);
	glBegin(GL_QUADS);
		glTexCoord2i(0,0);
		glVertex2i(-(_w/2),-(_h/2));

		glTexCoord2i(_w,0);
		glVertex2i(_w/2,-(_h/2));

		glTexCoord2i(_w,_h);
		glVertex2i(_w/2,_h/2);

		glTexCoord2i(0,_h);
		glVertex2i(-(_w/2),_h/2);
	glEnd();
	enddraw();
}

void Image::draw(int x, int y)
{
	if(!image)
		return;
	predraw(x+_w/2,y+_h/2);
	glBegin(GL_QUADS);
		glTexCoord2i(0,0);
		glVertex2i(-(_w/2),-(_h/2));

		glTexCoord2i(_w,0);
		glVertex2i(_w/2,-(_h/2));

		glTexCoord2i(_w,_h);
		glVertex2i(_w/2,_h/2);

		glTexCoord2i(0,_h);
		glVertex2i(-(_w/2),_h/2);
	glEnd();
	enddraw();
}

void Image::draw(int x, int y, int w, int h)
{
	if(!image)
		return;
	predraw(x+w/2,y+h/2);
	glBegin(GL_QUADS);
		glTexCoord2i(0,0);
		glVertex2i(-(w/2),-(h/2));

		glTexCoord2i(_w,0);
		glVertex2i(w/2,-(h/2));

		glTexCoord2i(_w,_h);
		glVertex2i(w/2,h/2);

		glTexCoord2i(0,_h);
		glVertex2i(-(w/2),h/2);
	glEnd();
	enddraw();
}

void Image::draw(int x, int y, double cx, double cy, double cw, double ch)
{
	if(!image)
		return;
	predraw(x+cw/2,y+ch/2);
	glBegin(GL_QUADS);
		glTexCoord2f(cx/_w,cy/_h);
		glVertex2i(-(cw/2),-(ch/2));

		glTexCoord2f((cx+cw)/_w,cy/_h);
		glVertex2i(cw/2,-(ch/2));

		glTexCoord2f((cx+cw)/_w,(cy+ch)/_h);
		glVertex2i(cw/2,ch/2);

		glTexCoord2f(cx/_w,(cy+ch)/_h);
		glVertex2i(-(cw/2),ch/2);
	glEnd();
	enddraw();
}

void Image::draw(int x, int y, int w, int h, double cx, double cy, double cw, double ch)
{
	predraw(x,y);
	glBegin(GL_QUADS);
		glTexCoord2i(cx,cy);
		glVertex2i(-(w/2),-(h/2));

		glTexCoord2i(cx+cw,cy);
		glVertex2i(w/2,-(h/2));

		glTexCoord2i(cx+cw,cy+ch);
		glVertex2i(w/2,h/2);

		glTexCoord2i(cx,cy+ch);
		glVertex2i(-(w/2),h/2);
	glEnd();
	enddraw();
}

int Image::W()
{
	return _w;
}

int Image::H()
{
	return _h;
}

void Image::set_tint(double r, double g, double b)
{
	_r = r;
	_g = g;
	_b = b;
}

void Image::rotation(double r)
{
	rot = r;
}

void Image::alpha(double a)
{
	this->a = a;
}

void Image::scale(double s)
{
	this->_scale = s;
}

void Image::predraw(int x, int y)
{
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, image);
	glLoadIdentity();
	glTranslatef(x,y,0);
	glRotatef(rot,0,0,1);
	glScalef(_scale, _scale, _scale);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glEnable(GL_BLEND);
	glColor4d(_r,_g,_b,a);
}

void Image::enddraw()
{
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}
GLuint Image::get_image()
{
	return image;
}
