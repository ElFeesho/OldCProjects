#include "entity.hpp"

using namespace SwordFeesh;

Entity::Entity(Scene *par) : x(0), y(0), w(0), h(0), parent(par)
{
}

Entity::Entity(double X, double Y, Scene *par) : x(X), y(Y), w(0), h(0), parent(par)
{
}

Entity::Entity(double X, double Y, double W, double H, Scene *par) : x(X), y(Y), w(W), h(H), parent(par)
{
}

Entity::~Entity()
{

}

void Entity::X(double x)
{
	this->x = x;
}

double Entity::X()
{
	return x;
}

void Entity::Y(double y)
{
	this->y = y;
}

double Entity::Y()
{
	return y;
}

void Entity::W(double w)
{
	this->w = w;
}

double Entity::W()
{
	return w;
}

void Entity::H(double h)
{
	this->h = h;
}

double Entity::H()
{
	return h;
}

void Entity::set_parent(Scene *parent)
{
	this->parent = parent;
}

Scene *Entity::get_parent()
{
	return parent;
}

double Entity::SX(Entity *container)
{
	if(container)
		return container->X()+x;
	return x;
}

double Entity::SY(Entity *container)
{
	if(container)
		return container->Y()+y;
	return y;
}

