#ifndef __ENTITYLIST_HPP__
#define __ENTITYLIST_HPP__

#include <vector>

#include "entity.hpp"

using std::vector;

namespace SwordFeesh
{
	class EntityList
	{
	public:
		EntityList();
		~EntityList();
		bool update(SDL_Surface *screen, Uint8 *keys);
		bool update();

		void add(Entity *ent);
	private:
		vector<Entity *> *ent_list;
	};
}

#endif
