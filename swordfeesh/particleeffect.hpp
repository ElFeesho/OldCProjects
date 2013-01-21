#ifndef __PARTICLE_EFFECT_HPP__
#define __PARTICLE_EFFECT_HPP__

namespace SwordFeesh
{
	class ParticleEffect
	{
	public:
		ParticleEffect();
		virtual ~ParticleEffect();
		virtual bool update() = 0;
	};
}

#endif
