#include "entitylist.hpp"

EntityList::EntityList()
{
	ent_list = new vector<Entity *>();
}

EntityList::~EntityList()
{
	for(vector<Entity *>::iterator it = ent_list->begin(); it!=ent_list->end();)
	{
		delete (*it);
		it = ent_list->erase(it);
	}

	ent_list->clear();

	delete ent_list;
}

bool EntityList::update()
{
	return update(SDL_GetVideoSurface(),SDL_GetKeyState(NULL));
}

bool EntityList::update(SDL_Surface *screen, Uint8 *keys)
{
	if(ent_list->size() == 0)
		return false;
	for(vector<Entity *>::iterator it = ent_list->begin(); it!=ent_list->end();)
	{
		if((*it)->update(keys))
		{
			it++;
		}
		else
		{
			delete (*it);
			it = ent_list->erase(it);
		}
	}
	return true;
}

void EntityList::add(Entity *ent)
{
	ent_list->push_back(ent);
}
