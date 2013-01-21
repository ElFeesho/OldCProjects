#include <swordfeesh.hpp>
#include "menu.hpp"

using SwordFeesh::Engine;

int main(int argc, char **argv)
{
	Engine *handle = new Engine(800,600);
	handle->set_scene(new Menu());
	while(handle->update())
		;

	return 0;
}
