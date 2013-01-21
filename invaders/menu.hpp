#ifndef __MENU_HPP__
#define __MENU_HPP__

#include <swordfeesh.hpp>

using namespace SwordFeesh;

class Menu : public Scene
{
public:
	Menu();
	~Menu();
	
	bool update(Uint8 *keys);
	
private:
	Image *title;
	ParticleFountain *pf;
};

#endif