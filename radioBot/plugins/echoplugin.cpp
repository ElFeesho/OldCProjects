#include "echoplugin.hpp"

#include <iostream>

using std::cout;
using std::endl;

EchoPlugin::EchoPlugin()
{
	name = "EchoPlugin";
}

EchoPlugin::~EchoPlugin()
{
	
}

void EchoPlugin::handle_msg(const string &sender, const string &target, const string &msg)
{
	cout << target.c_str() << ": <" << sender.c_str() << "> " << msg.c_str() << endl;
}

void EchoPlugin::handle_privmsg(const string &sender, const string &msg)
{
	cout << ">" << sender.c_str() << "< " << msg.c_str() << endl;
}

void EchoPlugin::handle_action(const string &who, const string &target, const string &action)
{
	cout << target.c_str() << ": * " << who.c_str() << " " << action.c_str() << endl;
}

void EchoPlugin::handle_join(const string &who, const string &channel)
{
	cout << channel.c_str() << ": * " << who.c_str() << " joins" << endl;
}

void EchoPlugin::handle_part(const string &who, const string &channel, const string &msg)
{
	cout << channel.c_str() << ": * " << who.c_str() << " parts: " << msg.c_str() << endl;
}

void EchoPlugin::handle_quit(const string &who, const string &msg)
{
	cout << who.c_str() << " quits: " << msg.c_str() << endl;
}

void EchoPlugin::plugin_loaded()
{
	cout << "EchoPlugin loaded" << endl;
}

void EchoPlugin::plugin_unloaded()
{
	cout << "EchoPlugin unloaded" << endl;
}

extern "C"
{
	Plugin *get_plugin()
	{
		return new EchoPlugin;
	}
	
	void destroy_plugin(Plugin *plug)
	{
		delete plug;
	}
}
