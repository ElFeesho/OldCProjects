#ifndef __PLUGIN_LOADER_HPP__
#define __PLUGIN_LOADER_HPP__

#include "plugin.hpp"

class PluginLoader : public Plugin
{
	public:
		PluginLoader();
		~PluginLoader();
		
		void handle_privmsg(const string &sender, const string &msg);

		void plugin_loaded();
		void plugin_unloaded();
};

#endif