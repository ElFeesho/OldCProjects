#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <SDL/SDL.h>

#include "entitylist.hpp"

namespace SwordFeesh
{

	class Scene
	{
	public:
		Scene();
		virtual ~Scene();
		virtual bool update(Uint8 *keys) = 0;
		EntityList *get_ent_list();
	private:
		EntityList *ent_list;
	};
}

#endif
