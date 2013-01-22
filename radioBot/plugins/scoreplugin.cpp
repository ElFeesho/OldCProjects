#include "scoreplugin.hpp"

#include <iostream>
using std::cout;
using std::endl;

ScorePlugin::ScorePlugin()
{
	name = "ScorePlugin";
	clients = new vector<Client *>();
}

ScorePlugin::~ScorePlugin()
{
	delete clients;
}

void ScorePlugin::handle_privmsg(const string &sender, const string &msg)
{
}

void ScorePlugin::handle_join(const string &who, const string &channel)
{
	bool found = false;
	for(int i = 0; i<clients->size();i++)
	{
		if(((*clients)[i])->getNick() == who)
			found = true;
	}

	if(!found)
	{
		clients->push_back(new Client(who));
	}
}

void ScorePlugin::update()
{
	
}

void ScorePlugin::plugin_loaded()
{
	cout << "ScorePlugin Loaded" << endl;
}

void ScorePlugin::plugin_unloaded()
{
	cout << "ScorePlugin Unloaded" << endl;
}

extern "C"
{
	Plugin *get_plugin()
	{
		return new ScorePlugin;
	}
	
	void destroy_plugin(Plugin *plug)
	{
		delete plug;
	}
}
