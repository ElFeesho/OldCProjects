#include "entity.hpp"

Entity::Entity() : x(0), y(0)
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

