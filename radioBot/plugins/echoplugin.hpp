#ifndef __ECHOPLUGIN_HPP__
#define __ECHOPLUGIN_HPP__

#include "plugin.hpp"

class EchoPlugin : public Plugin
{
	public:
		EchoPlugin();
		~EchoPlugin();
		
		void handle_msg(const string &sender, const string &target, const string &msg);
		void handle_privmsg(const string &sender, const string &msg);
		void handle_action(const string &who, const string &target, const string &action);
		void handle_join(const string &who, const string &channel);
		void handle_part(const string &who, const string &channel, const string &msg);
		void handle_quit(const string &who, const string &msg);
		
		void plugin_loaded();
		void plugin_unloaded();
};

#endif
