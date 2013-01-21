#include <iostream>
#include "image.hpp"

using namespace std;

Image::Image():image(NULL)
{
	
}

Image::Image(const char *filename):image(IMG_Load(filename))
{
	if(!image)
	{
		cout << "FAIL! " << filename << endl;
	}
}

Image::~Image()
{
	if(image)
		SDL_FreeSurface(image);
}

void Image::draw()
{
	if(!image)
		return;
	SDL_Surface *screen = SDL_GetVideoSurface();
	SDL_BlitSurface(image,NULL,screen,NULL);
}

void Image::draw(int x, int y)
{
	if(!image)
		return;
	SDL_Surface *screen = SDL_GetVideoSurface();
	SDL_Rect pos = { x, y, image->w, image->h };
	SDL_BlitSurface(image,NULL,screen,&pos);
}

void Image::draw(int x, int y, int w, int h)
{
	if(!image)
		return;
		SDL_Surface *screen = SDL_GetVideoSurface();
	SDL_Rect pos = { x, y, w, h };
	SDL_BlitSurface(image,NULL,screen,&pos);
}

void Image::draw(int x, int y, int cx, int cy, int cw, int ch)
{
	if(!image)
		return;
	SDL_Surface *screen = SDL_GetVideoSurface();
	SDL_Rect pos = { x, y, image->w, image->h };
	SDL_Rect cpos = { cx, cy, cw, ch };
	SDL_BlitSurface(image,&cpos,screen,&pos);
}

void Image::draw(int x, int y, int w, int h, int cx, int cy, int cw, int ch)
{
	if(!image)
		return;
	SDL_Surface *screen = SDL_GetVideoSurface();
	SDL_Rect pos = { x, y, w, h };
	SDL_Rect cpos = { cx, cy, cw, ch };
	SDL_BlitSurface(image,&cpos,screen,&pos);
}

int Image::w()
{
	return image->w;	
}

int Image::h()
{
	return image->h;
}
