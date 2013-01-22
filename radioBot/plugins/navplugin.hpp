#ifndef __NAV_PLUGIN_HPP__
#define __NAV_PLUGIN_HPP__

#include "plugin.hpp"

class NavPlugin : public Plugin
{
	public:
		NavPlugin();
		~NavPlugin();
		
		void handle_privmsg(const string &sender, const string &msg);
		void update();
		
		void plugin_loaded();
		void plugin_unloaded();
};

#endif