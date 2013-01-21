#include <cstdio>
#include <cstdlib>
#include "engine.hpp"
#include "particlefountain.hpp"

using namespace SwordFeesh;

ParticleFountain::ParticleFountain(const char *filename, int count, long int TTL, double X, double Y, double R, double G, double B) : part_count(count), ttl(TTL), x(X), y(Y), particle_image(new Image(filename)), r(R), g(G), b(B)
{
	image = particle_image->get_image();
	particles = new Particle::FountainParticle*[part_count];
	for(int i = 0;i<part_count;i++)
	{
		particles[i] = new Particle::FountainParticle(image, (float)TTL*((float)i/part_count), x, y, r, g, b);
	}
}

ParticleFountain::~ParticleFountain()
{
	for(int i = 0;i<part_count;i++)
		delete particles[i];
	delete particles;
	delete particle_image;
}

void ParticleFountain::set_count(int count)
{
	part_count = count;
}

void ParticleFountain::set_ttl(long int TTL)
{
	ttl = TTL;
}

void ParticleFountain::set_pos(double X, double Y)
{
	x = X;
	y = Y;
}

void ParticleFountain::X(double x)
{
	this->x = x;
}

void ParticleFountain::Y(double y)
{
	this->y = y;
}

double ParticleFountain::X()
{
	return x;
}

double ParticleFountain::Y()
{
	return y;
}

bool ParticleFountain::update()
{
	glLoadIdentity();
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, image);

	for(int i = 0; i<part_count;i++)
	{
		if(particles[i] == NULL) continue;
		if(!particles[i]->update())
		{
			delete particles[i];
			particles[i] = NULL;
			particles[i] = new Particle::FountainParticle(image, ttl, x, y,r,g,b);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	return true;
}

/*
	PARTICLE::FOUNTAINPARTICLE
*/

Particle::FountainParticle::FountainParticle(GLuint tex, long int TTL, double X, double Y, double R, double G, double B) : x(X), y(Y), xspeed(0), yspeed(0), ttl(TTL), image(tex), r(R), g(G), b(B)
{
	x += (rand()%32)-16;
	y += (rand()%32)-16;

	while(xspeed==0)
		xspeed = ((rand()%20)-10)/20.0f;

	while(yspeed==0)
		yspeed = ((rand()%20))/5.0f;
	bt = Engine::get_inst()->get_ticks();
}

Particle::FountainParticle::~FountainParticle()
{

}

bool Particle::FountainParticle::update()
{
	// TODO: draw and do physics bitch
	long int ticks = Engine::get_inst()->get_ticks()-bt;
	glColor4f(r, g, b,1.0f - (ticks/(float)ttl));
	glBegin(GL_QUADS);
		glTexCoord2i(0,0);
		glVertex2d(x-16,y-16);
		glTexCoord2i(32,0);
		glVertex2d(x+16,y-16);
		glTexCoord2i(32,32);
		glVertex2d(x+16,y+16);
		glTexCoord2i(0,32);
		glVertex2d(x-16,y+16);
	glEnd();
	y+=yspeed;
	x+=xspeed;
	if(ticks>ttl)
	{
		return false;
	}
	return true;
}
