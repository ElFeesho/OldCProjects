#include "navplugin.hpp"

#include <iostream>

using std::cout;
using std::endl;

NavPlugin::NavPlugin()
{
	name = "NavPlugin";
}

NavPlugin::~NavPlugin()
{

}

void NavPlugin::handle_privmsg(const string &sender, const string &msg)
{
	if(sender.compare(string("feesh"))!=0)
		return;
	
	if(msg.find("!part")==0)
	{
			irc->part(msg.substr(6));
		
	}
	else if(msg.find("!join")==0)
	{
		irc->join(msg.substr(6));
	}
	else if(msg.find("!quit")==0)
	{
		irc->quit("Goodbye everyone!");
	}
	else if(msg.find("!login") == 0)
	{
		irc->send_privmsg("identify vagooboo", "nickserv");
		cout << "Registered..." << endl;
	}
	else if(msg.find("!register") == 0)
	{
		irc->send_privmsg("register vagooboo", "nickserv");
		cout << "Logged in." << endl;
	}
}

void NavPlugin::update()
{
	
}

void NavPlugin::plugin_loaded()
{
	cout << "NavPlugin Loaded" << endl;
}

void NavPlugin::plugin_unloaded()
{
	cout << "NavPlugin Unloaded" << endl;
}

extern "C"
{
	Plugin *get_plugin()
	{
		return new NavPlugin;
	}
	
	void destroy_plugin(Plugin *plug)
	{
		delete plug;
	}
}
