#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <swordfeesh.hpp>
#include "menu.hpp"

using SwordFeesh::Engine;

int main(int argc, char **argv)
{
	srand(time(NULL));
	Engine *handle = new Engine(1024,768);
//	handle->toggle_fullscreen();
	handle->set_name("Feeshtris!");
	handle->hide_mouse(true);
	handle->set_scene(new Menu());
	while(handle->update())
		;
	handle->hide_mouse(false);
//	handle->toggle_fullscreen();
	return 0;
}
