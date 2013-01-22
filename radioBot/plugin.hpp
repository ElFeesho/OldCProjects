#ifndef __PLUGIN_HPP__
#define __PLUGIN_HPP__

#include "irc.hpp"

class Plugin
{
	public:
		Plugin();
		virtual ~Plugin();
		
		void *get_handle();
		string get_name();
		
		void bootstrap(void *handle, IRC *irc);
		
		virtual void handle_msg(const string &sender, const string &target, const string &msg);
		virtual void handle_privmsg(const string &sender, const string &msg);
		virtual void handle_action(const string &who, const string &target, const string &action);
		virtual void handle_join(const string &who, const string &channel);
		virtual void handle_part(const string &who, const string &channel, const string &msg);
		virtual void handle_quit(const string &who, const string &msg);
		
		virtual void update();
		
		virtual void plugin_loaded() = 0;
		virtual void plugin_unloaded() = 0;
		
	protected:
		IRC *irc;
		string name;
	private:
		void *_handle;
};

#endif