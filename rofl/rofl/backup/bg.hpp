#ifndef __BG_HPP__
#define __BG_HPP__

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "entity.hpp"

#include "image.hpp"

class BG : public Entity
{
public:
	BG();
	~BG();
	void track(Entity *ent);
	bool update(Uint8 *keys);
private:
	Image *bg;
};

#endif
