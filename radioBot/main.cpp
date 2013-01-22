#include <iostream>
#include <string>
#include "irc.hpp"

using namespace std;

int main(int argc, char **argv)
{
	cout << ":: RadioBot startup" << endl;

	// Load plugins

	IRC *irc = new IRC();
	irc->load_plugin("./pluginloader");

	if(irc->connect(string("irc.freenode.net"), "6667"))
	{
			cout << ":: Connected to host" << endl;
	}
	else
	{
		cerr << ":: Couldn't connect to host - Does it exist?" << endl;
	}
	cout << ":: Sending details" << endl;
	irc->set_details(string("radiobot"),string("radiobots"),string("radiobotbot"));
	cout << ":: Sending nick" << endl;
	irc->nick("feeshbot");
	cout << ":: Sending identity" << endl;
	//irc->send_privmsg("identify elfeesho feeshily","drone");
	//irc->send_privmsg("identify vagooboo","nickserv");
	cout << ":: Joining channel" << endl;
	irc->join("##lovepalacedelfeesh");
	cout << ":: Ready" << endl;

	while(irc->is_active())
	{
		irc->update();
	}

	cout << ":: RadioBot shutdown" << endl;
	return 0;
}
