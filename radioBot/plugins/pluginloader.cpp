#include "pluginloader.hpp"

#include <iostream>

using std::cout;
using std::endl;

PluginLoader::PluginLoader()
{
	name = "PluginLoader";
}

PluginLoader::~PluginLoader()
{

}

void PluginLoader::handle_privmsg(const string &sender, const string &msg)
{
	if(sender.compare(string("feesh"))!=0)
		return;
	
	if(msg.find("!load")==0)
	{
		cout << "Loading plugin" << endl;
		// Loading a plugin
		if(irc->load_plugin(msg.substr(6)))
		{
			// attempt to send a privmsg to the sender
			irc->send_privmsg("Plugin loaded",sender);
		}
		else
		{
			irc->send_privmsg("Plugin couldn't be loaded, perhaps you misspelt the filename?",sender);
		}
	}
	else if(msg.find("!unload")==0)
	{
		cout << "Unloading plugin" << endl;
		if(irc->unload_plugin(msg.substr(8))) // past the '!unload '
		{
			irc->send_privmsg("Plugin unloaded",sender);
		}
		else
		{
			irc->send_privmsg("Plugin couldn't be unloaded, perhaps you misspelt it?",sender);
		}
	}
}


void PluginLoader::plugin_loaded()
{
	cout << "PluginLoader Loaded" << endl;
}

void PluginLoader::plugin_unloaded()
{
	cout << "PluginLoader Unloaded" << endl;
}

extern "C"
{
	Plugin *get_plugin()
	{
		return new PluginLoader;
	}
	
	void destroy_plugin(Plugin *plug)
	{
		delete plug;
	}
}
