#include "plugin.hpp"

#include <iostream>

using std::cout;

Plugin::Plugin()
{
	name = "Unnamed Plugin";
}

Plugin::~Plugin()
{
	
}

void *Plugin::get_handle()
{
	return _handle;
}

string Plugin::get_name()
{
	return name;
}

void Plugin::bootstrap(void *handle, IRC *irc)
{
	_handle = handle;
	this->irc = irc;
}

void Plugin::handle_msg(const string &sender, const string &target, const string &msg)
{
	
}

void Plugin::handle_privmsg(const string &sender, const string &msg)
{

}

void Plugin::handle_action(const string &who, const string &target, const string &action)
{

}

void Plugin::handle_join(const string &who, const string &channel)
{

}

void Plugin::handle_part(const string &who, const string &channel, const string &msg)
{

}

void Plugin::handle_quit(const string &who, const string &channel)
{

}

void Plugin::update()
{
	
}