#include <iostream>
#include <wiiboards.hpp>

#include "sync.hpp"

using namespace std;

Sync::Sync()
{
	bg = new Image("images/sync.png");
//	InputEvent::init_wiiboards();
}

Sync::~Sync()
{

}

bool Sync::update(Uint8 *key)
{
	static int boards_connected = 0;

//	if(Wiiboards::get_connected()>boards_connected)
//	{
//		boards_connected = Wiiboards::get_connected();
//		cout << "Boards Connected: " << boards_connected << endl;
//	}
	bg->draw();
	return true;
}
