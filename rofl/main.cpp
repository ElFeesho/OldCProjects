#include <engine.hpp>
#include "sync.hpp"

int main(int argc, char **argv)
{
	Engine *gameOne = new Engine(800, 600);
	gameOne->set_scene(new Sync());
	while(gameOne->update());
	
	delete gameOne;

	return 0;
}
