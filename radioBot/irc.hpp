#ifndef __IRC_HPP__
#define __IRC_HPP__

#include "net.hpp"

#include <string>
#include <vector>

using std::string;
using std::vector;

class Plugin;

class IRC
{
	public:
		IRC();
		~IRC();
		
		bool is_active();
		
		bool connect(const string &host, const string &port);
		
		void set_details(const string &name, const string &user, const string &pass);
		
		void nick(const string &nick);
		void join(const string &channel);
		void part(const string &channel);
		void quit(const string &msg);
		
		void send_privmsg(const string &message, const string &target);
		
		bool load_plugin(const string &file_name);
		bool unload_plugin(const string &plugin_name);

		void update();
		
		string get_nick();
		
		string get_msg();
	private:
		Net *connection;
		string _nick;
		
		// Pointless login crap
		string name;
		string username;
		string password;
				
		vector<Plugin *> plugins;
		vector<string> channels;
		
		bool active;
};

#endif
