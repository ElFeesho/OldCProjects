#include "SDLW.hpp"

void SDLW::draw_rect(int x, int y, int w, int h, char r, char g, char b)
{
	SDL_Rect drect;
	drect.x = x;
	drect.y = y;
	drect.w = w;
	drect.h = h;
	SDL_FillRect(Engine::get_buffer(),&drect,SDL_MapRGB(Engine::get_screen()->format,r,g,b));
}

void SDLW::draw_surf(SDL_Surface *src, SDL_Surface *dest)
{
	SDL_BlitSurface(src, NULL, dest, NULL);
}

void SDLW::draw_surf(SDL_Surface *src, SDL_Surface *dest, int x, int y)
{
	SDL_Rect drect;
	drect.x = x;
	drect.y = y;
	drect.w = src->w;
	drect.h = src->h;
	SDL_BlitSurface(src, NULL, dest, &drect);
}

void SDLW::draw_black()
{
	SDLW::draw_rect(0,0,Engine::get_buffer()->w,Engine::get_buffer()->h,0,0,0);
}
