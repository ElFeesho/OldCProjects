#include <swordfeesh.hpp>
#include "menusel.hpp"

using namespace SwordFeesh;

MenuSel::MenuSel()
{
	menu_sel = new Image("images/menusel.png");
}

MenuSel::MenuSel(double x, double y) : x(x), y(y), dx(x), dy(y)
{
	menu_sel = new Image("images/menusel.png");
}

MenuSel::~MenuSel()
{

}

bool MenuSel::update(Uint8 *keys)
{
	if(x-dx<-0.1 || x-dx>0.1)
		x+=(dx-x)/2;

	if(y-dy<-0.1 || y-dy>0.1)
		y+=(dy-y)/2;
	menu_sel->draw(x, y);
	return true;
}

void MenuSel::set_pos(double x, double y)
{
	dx = x;
	dy = y;
}
