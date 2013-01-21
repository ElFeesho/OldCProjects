#include "main_desktop.hpp"
#include "locator.hpp"

int main(int argc, char **argv)
{
	MainDesktop desktop;
	Locator test;
	for(;;)
	{
		test.update();
		desktop.flush();
	}
	return 0;
}
