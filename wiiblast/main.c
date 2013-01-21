#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <wiiuse.h>

int main(int argc, char **argv)
{
	init_sdl();
	for(;;)
	{
		find_wiimotes();
	}
	return 0;
}
