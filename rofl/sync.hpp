#ifndef __SYNC_HPP__
#define __SYNC_HPP__

#include <scene.hpp>
#include <image.hpp>
#include <inputevent.hpp>
#include <SDL/SDL.h>

class Sync : public Scene
{
public:
	Sync();
	~Sync();
	bool update(Uint8 *keys);
private:
	Image *bg;
};

#endif

