#ifndef __MENU_SEL_HPP__
#define __MENU_SEL_HPP__

#include <swordfeesh.hpp>

class MenuSel : public SwordFeesh::Entity
{
public:
	MenuSel();
	MenuSel(double x, double y);
	~MenuSel();
	bool update(Uint8 *keys);
	void set_pos(double x, double y);
private:
	double dx, dy;
	double x, y;
	SwordFeesh::Image *menu_sel;
};

#endif
