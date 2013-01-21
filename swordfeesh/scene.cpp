#include "scene.hpp"

using namespace SwordFeesh;

Scene::Scene()
{
	ent_list = new EntityList();
}

Scene::~Scene()
{

}

EntityList *Scene::get_ent_list()
{
	return ent_list;
}
