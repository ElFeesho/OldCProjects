#include "irc.hpp"
#include "plugin.hpp"

#include <iostream>
#include <dlfcn.h>

string get_sender(string line);
string get_recipient(string line);

typedef Plugin *plugin_load_t();
typedef void plugin_unload_t(Plugin *p);

using namespace std;

IRC::IRC()
{
	active = false;
}

IRC::~IRC()
{
	connection->shutdown();
	delete connection;
}

bool IRC::is_active()
{
	return active;
}

bool IRC::connect(const string &host, const string &port)
{
	connection = new Net();
	return active = connection->connect(host, port);
}

void IRC::set_details(const string &name, const string &user, const string &pass)
{
	this->name = name;
	this->username = user;
	this->password = pass;
	
	string request = string("PASS ")+password+string("\r\n");
	connection->send_line(request.c_str());
	
	string request_two = string("USER ")+user+string(" 0 * :")+name+string("\r\n");
	connection->send_line(request_two.c_str()); // Send the \0
}

void IRC::nick(const string &nick)
{
	this->_nick = nick;
	string request = string("NICK ")+nick+string("\r\n");
	
	connection->send_line(request.c_str());
}

void IRC::join(const string &channel)
{
	for(int i = 0;i<channels.size();i++)
	{
		if(channels[i].compare(channel)==0)
		{
			// Already in this channel, do nothing
			return;
		}
	}
	channels.push_back(channel);
	string request = string("JOIN ")+channel+string("\r\n");
	connection->send_line(request.c_str());
}

void IRC::part(const string &channel)
{
	for(vector<string>::iterator  it = channels.begin();it!=channels.end();it++)
	{
		if((*it).compare(channel)==0)
		{
			// The bot is in that channel! not for long!
			string request = string("PART ")+channel+string(" :\"Good Bye!\"\r\n");
			connection->send_line(request.c_str());
			channels.erase(it);
			return;
		}
	}
}

void IRC::quit(const string &msg)
{
	string request = string("QUIT :")+msg+string("\r\n");
	connection->send_line(request.c_str());
	connection->shutdown();
	active = false;
}

void IRC::update()
{
	if(connection->can_read())
	{
		get_msg();
	}
	for(int i = 0;i<plugins.size();i++)
	{
		plugins[i]->update();
	}
}

void IRC::send_privmsg(const string &message, const string &target)
{
	string request = string("PRIVMSG ")+target+string(" :")+message+string("\r\n");
	connection->send_line(request.c_str());
}

bool IRC::load_plugin(const string &file_name)
{
	Plugin *nplug;
	void *handle = dlopen(file_name.c_str(),RTLD_LAZY);
	if(!handle)
	{
		cerr << "Couldn't load plugin: "<<file_name.c_str() << ": " << dlerror() << endl;
		return false;
	}
	
   dlerror();
	
	plugin_load_t *create_plugin = (plugin_load_t*) dlsym(handle, "get_plugin");
	if(dlerror())
	{
		cerr << "Couldn't locate get_plugin in the plugin" << endl;
		return false;
	}
	
	nplug = create_plugin();
	nplug->bootstrap(handle, this);
	nplug->plugin_loaded();
	
	// Make sure the plugin is not already loaded...
	for(vector<Plugin *>::iterator it = plugins.begin(); it!=plugins.end();it++)
	{
		if((*it)->get_name().compare(nplug->get_name()) == 0)
		{
			plugin_unload_t *unload_plugin = (plugin_unload_t*) dlsym(handle,"destroy_plugin");
			if(dlerror())
			{
				cerr << "Couldn't get plugin's destroy function... memory leak will occur" << endl;
			}
			else
			{
				unload_plugin((*it));
			}
			
			dlclose(handle);
			cout << "Plugin unloaded - it has already been loaded!" << endl;
			return false;
		}
	}
	plugins.push_back(nplug);
	cout << "Successfully loaded " << nplug->get_name().c_str() << endl;
	return true;
}

bool IRC::unload_plugin(const string &plugin_name)
{
	// iterate through plugins loaded, find their unload function, call it and destroy it
	if(plugins.size()<=0)
		return false; // no plugins to unload...
		
	for(vector<Plugin *>::iterator it = plugins.begin(); it!=plugins.end();it++)
	{
		if(plugin_name.compare((*it)->get_name())==0)
		{
			// found it
			void *handle = (*it)->get_handle();
			
			(*it)->plugin_unloaded();
			
			plugin_unload_t *unload_plugin = (plugin_unload_t*) dlsym(handle,"destroy_plugin");
			if(dlerror())
			{
				cerr << "Couldn't get plugin's destroy function... memory leak will occur" << endl;
			}
			else
			{
				unload_plugin((*it));
			}
			
			dlclose(handle);
			cout << "Plugin unloaded" << endl;
			plugins.erase(it);
			return true;
			break;
		}
	}
	return false;
}

string IRC::get_nick()
{
	return _nick;
}

string IRC::get_msg()
{
	string ret_val = connection->read_line();

	// Check for pings
	if(ret_val.find("PING :")==0)
	{
		string server = ret_val.substr(6);
		// respond to the pings
		string request = string("PONG ")+server; // will have \r\n on the end already
		connection->send_line(request.c_str());
		return ret_val;
	}
	
	if(plugins.size()<=0)
	{
		return ret_val; // No point doing any other calculations if there's no plugins loaded...
	}
	
	// PRIVATE MESSAGE :feesh!~feesh@unaffiliated/feesh PRIVMSG SickSickSix :nipple boob
	// ACTION :feesh!~feesh@unaffiliated/feesh PRIVMSG ##scifi :\x01ACTION\x01 waves
	// JOIN 
	
	// JOIN :feesh!~feesh@unaffiliated/feesh JOIN :##scifi

	// Find out what kind of message we're dealing with
	
	// Find out the location of the second colon - remove the first colon first
	
	ret_val = ret_val.substr(1);
	int colon_pos = ret_val.find(":");
	
	string command = ret_val.substr(0,colon_pos);
	string data = ret_val.substr(colon_pos+1);
	data = data.substr(0,data.length()-2); // Remove the \r\n
	
	if(command.find(_nick)==0)
	{
			// Something the bot has done...
			
			if(command.find("JOIN")!=string::npos)
			{
				// Bot has susccessfully joined a channel!
				channels.push_back(data);
			}
	}
	
	//cout << "Command: "<<command.c_str()<<endl;
	//cout << "Data: "<<data.c_str() << endl;
	
	int type = 0;
	if(command.find("PRIVMSG")!=string::npos)
		type = 1;
	else if(command.find(" JOIN ")!=string::npos)
		type = 2;
	else if(command.find(" PART ")!=string::npos)
		type = 3;
	else if(command.find(" QUIT ")!=string::npos)
		type = 4;
	else if(command.find(" NOTICE ")!=string::npos)
		type = 5;

	// Dopn't know what it is! - Numeric reply no doubt
	if(type == 0)
	{
		return ret_val;
	}
	else if(type == 1)
	{
		string sender = get_sender(command);
		string recipient = get_recipient(command);
		bool privmsg = recipient.compare(_nick)==0;
		if(data.at(0) == '\x01' && data.find("ACTION ")==1 && data.at(data.length()-1) == '\x01') // have an action!
		{
			data = data.substr(8,data.length()-9);
			for(int i = 0; i<plugins.size(); i++)
			{
				plugins[i]->handle_action(sender, recipient, data);
			}
		}
		else
		{
			for(int i = 0; i<plugins.size(); i++)
			{
				if(privmsg)
					plugins[i]->handle_privmsg(sender, data);
				else
					plugins[i]->handle_msg(sender, recipient, data);
			}
		}
	}
	else if(type == 2)
	{
		string joiner = get_sender(command);
		for(int i = 0; i<plugins.size(); i++)
		{
			plugins[i]->handle_join(joiner, data);
		}
	}
	else if(type == 3)
	{
		// Part :feesh!~feesh@unaffiliated/feesh PART ##scifi :"Leaving"
		string leaver = get_sender(command);
		string channel = get_recipient(command);
		for(int i = 0; i<plugins.size(); i++)
		{
			plugins[i]->handle_part(leaver, channel, data);
		}
	}
	else if(type == 4)
	{
		// Quit :feesh!~feesh@unaffiliated/feesh QUIT ##scifi :"Leaving"
		string leaver = get_sender(command);
		for(int i = 0; i<plugins.size(); i++)
		{
			plugins[i]->handle_quit(leaver, data);
		}
	}
	else if(type == 5)
	{
		// Plugins don't get notices ... yet
	}
	
	return ret_val;
}

string get_sender(string line)
{
	string sender = line.substr(0,line.find(" ")); // remove the first :
	
	// Can we find out if it's a server or not? I THINK SO!
	if(sender.find("!")!=string::npos) // We've got a nick name here
	{
		sender = sender.substr(0,sender.find("!"));
	}
	else
	{
		sender = sender.substr(0,sender.find(" "));
	}
	
	return sender;
}

string get_recipient(string line)
{
	string recipient = line.substr(0,line.length()-1); // remove the last space
	recipient = recipient.substr(recipient.find_last_of(" ")+1);
	return recipient;
}