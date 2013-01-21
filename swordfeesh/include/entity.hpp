#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

#include <SDL/SDL.h>

namespace SwordFeesh
{
	class Scene;

	class Entity
	{
	public:
		Entity(Scene *par = NULL);
		Entity(double X, double Y, Scene *par = NULL);
		Entity(double X, double Y, double W, double H, Scene *par = NULL);
		virtual ~Entity();

		virtual bool update(Uint8 *keys) = 0;

		void X(double x);
		double X();

		void Y(double y);
		double Y();

		void W(double w);
		double W();

		void H(double h);
		double H();

		void set_parent(Scene *parent);
		Scene *get_parent();

		double SX(Entity *container);
		double SY(Entity *container);

	private:	
		double x, y;
		double w, h;
		Scene *parent;
	};
}

#endif
