#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <SDL/SDL.h>

class Scene
{
public:
	Scene();
	virtual ~Scene();
	virtual bool update(Uint8 *keys) = 0;
private:
};

#endif
