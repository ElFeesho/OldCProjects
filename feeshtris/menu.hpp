#ifndef __MENU_HPP__
#define __MENU_HPP__

#include <swordfeesh.hpp>

#include "menusel.hpp"

class Menu : public SwordFeesh::Scene
{
public:
	Menu();
	~Menu();
	bool update(Uint8 *keys);
private:
	SwordFeesh::Image *title;
	SwordFeesh::Image *menu_txt;
	SwordFeesh::EntityList *bubbles;
	MenuSel *menu_sel;
	int menuopt;
};

#endif
