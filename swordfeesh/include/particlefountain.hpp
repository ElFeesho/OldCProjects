#ifndef __PARTICLE_FOUNTAIN_HPP__
#define __PARTICLE_FOUNTAIN_HPP__

#include "particleeffect.hpp"
#include "image.hpp"
#include <vector>

using std::vector;

namespace SwordFeesh
{

	namespace Particle
	{
		class FountainParticle
		{
		public:
			FountainParticle();
			FountainParticle(GLuint tex, long int TTL, double X, double Y, double R = 1, double G = 1, double B = 1);
			~FountainParticle();
			bool update();
		private:
			double x, y;
			long int ttl;
			long int bt;
			double xspeed, yspeed;
			GLuint image;
			double r, g, b;
		};
	}

	class ParticleFountain : public ParticleEffect
	{
	public:
		ParticleFountain(const char *filename, int count, long int TTL, double X, double Y, double R = 1, double G = 1, double B = 1);
		~ParticleFountain();
		bool update();
		void set_count(int count);
		void set_ttl(long int TTL);
		void set_pos(double X, double Y);
		void X(double x);
		void Y(double y);
		double X();
		double Y();
	private:
		Particle::FountainParticle **particles;
		int part_count;
		long int ttl;
		double x, y;
		double r, g, b;
		Image *particle_image;
		GLuint image;
	};
}

#endif
