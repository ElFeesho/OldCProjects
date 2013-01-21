#include <swordfeesh.hpp>
#include "menu.hpp"

using namespace SwordFeesh;

int main(int argc, char **argv)
{
	Engine *eng = new Engine(640,480);
	
	Menu *menu = new Menu();
	
	eng->set_scene(menu);
	
	while(eng->update())
	{
	}
	
	return 0;
}