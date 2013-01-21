#ifndef __MENU_HPP__
#define __MENU_HPP__

#include <SDL/SDL.h>

#include "scene.hpp"
#include "image.hpp"

class Menu : public Scene
{
public:
	Menu();
	~Menu();
	bool update(Uint8 *keys);
private:
	Image *menu_img;
	Image *sel_item;
	unsigned int sel_choice;
};

#endif
